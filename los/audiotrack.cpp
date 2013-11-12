//=========================================================
//  LOS
//  Libre Octave Studio
//  $Id: audiotrack.cpp,v 1.14.2.21 2009/12/20 05:00:35 terminator356 Exp $
//
//  (C) Copyright 2004 Werner Schweer (ws@seh.de)
//=========================================================

#include <values.h>
#include <stdlib.h>
#include <map>

#include <QFile>
#include <QMessageBox>

#include "track.h"
#include "event.h"
#include "song.h"
#include "audio.h"
#include "wave.h"
#include "xml.h"
#include "audiodev.h"
#include "midictrl.h"
#include "mididev.h"
#include "midiport.h"
#include "midimonitor.h"
#include "globals.h"


//---------------------------------------------------------
//   AudioTrack
//---------------------------------------------------------

AudioTrack::AudioTrack(TrackType t)
: Track(t)
{
    _processed = false;
    _haveData = false;
    _prefader = false;
    _recFile = 0;
    _channels = 0;
    _automationType = AUTO_OFF;
    setChannels(2);
    addController(new CtrlList(AC_VOLUME,"Volume", 0.001, 3.16 /* roughly 10 db */));
    addController(new CtrlList(AC_PAN, "Pan", -1.0, 1.0));
    addController(new CtrlList(AC_MUTE, "Mute", 0.0, 1.0, true /*dont show in Composer */));

    _totalOutChannels = MAX_CHANNELS;
    outBuffers = new float*[_totalOutChannels];
    for (int i = 0; i < _totalOutChannels; ++i)
        posix_memalign((void**) &outBuffers[i], 16, sizeof (float) * segmentSize);

    // This is only set by multi-channel syntis...
    _totalInChannels = 0;

    bufferPos = MAXINT;

    setVolume(1.0, true);
}

AudioTrack::AudioTrack(const AudioTrack& t, bool cloneParts)
: Track(t, cloneParts)
{
    _totalOutChannels = t._totalOutChannels; // Is either MAX_CHANNELS, or custom value (used by syntis).
    _processed = false;
    _haveData = false;
    _controller = t._controller;
    _prefader = t._prefader;
    _automationType = t._automationType;
    //FIXME:Update this to create new input/output tracks and connect them to the same routes
    _inRoutes = t._inRoutes;
    _outRoutes = t._outRoutes;

    int chans = _totalOutChannels;
    // Number of allocated buffers is always MAX_CHANNELS or more, even if _totalOutChannels is less.
    if (chans < MAX_CHANNELS)
        chans = MAX_CHANNELS;
    outBuffers = new float*[chans];
    for (int i = 0; i < chans; ++i)
        posix_memalign((void**) &outBuffers[i], 16, sizeof (float) * segmentSize);

    bufferPos = MAXINT;
    _recFile = t._recFile;
}

AudioTrack::~AudioTrack()
{
    int chans = _totalOutChannels;
    // Number of allocated buffers is always MAX_CHANNELS or more, even if _totalOutChannels is less.
    if (chans < MAX_CHANNELS)
        chans = MAX_CHANNELS;
    for (int i = 0; i < chans; ++i)
    {
        if (outBuffers[i])
            free(outBuffers[i]);
    }
    delete[] outBuffers;

}

//---------------------------------------------------------
//   newPart
//---------------------------------------------------------

Part* AudioTrack::newPart(Part*, bool /*clone*/)
{
    return 0;
}

//---------------------------------------------------------
//   addController
//---------------------------------------------------------

void AudioTrack::addController(CtrlList* list)
{
    _controller.add(list);
}

//---------------------------------------------------------
//   removeController
//---------------------------------------------------------

void AudioTrack::removeController(int id)
{
    iCtrlList i = _controller.find(id);
    if (i == _controller.end())
    {
        printf("AudioTrack::removeController id %d not found\n", id);
        return;
    }
    _controller.erase(i);
}

//---------------------------------------------------------
//   swapControllerIDX
//---------------------------------------------------------

void AudioTrack::swapControllerIDX(int idx1, int idx2)
{
    // FIXME This code is ugly.
    // At best we would like to modify the keys (IDXs) in-place and
    //  do some kind of deferred re-sort, but it can't be done...

    if (idx1 == idx2)
        return;

    if (idx1 < 0 || idx2 < 0)
        return;

    CtrlList *cl;
    CtrlList *newcl;
    int id1 = (idx1 + 1) * AC_PLUGIN_CTL_BASE;
    int id2 = (idx2 + 1) * AC_PLUGIN_CTL_BASE;
    int i, j;

    CtrlListList tmpcll;
    CtrlVal cv(0, 0.0);

    for (ciCtrlList icl = _controller.begin(); icl != _controller.end(); ++icl)
    {
        cl = icl->second;
        i = cl->id() & AC_PLUGIN_CTL_ID_MASK;
        j = cl->id() & ~((unsigned long) AC_PLUGIN_CTL_ID_MASK);
        if (j == id1 || j == id2)
        {
            newcl = new CtrlList(i | (j == id1 ? id2 : id1));
            newcl->setMode(cl->mode());
            newcl->setValueType(cl->valueType());
            newcl->setName(cl->name());
            double min, max;
            cl->range(&min, &max);
            newcl->setRange(min, max);
            newcl->setCurVal(cl->curVal());
            newcl->setDefault(cl->getDefault());
            for (iCtrl ic = cl->begin(); ic != cl->end(); ++ic)
            {
                cv = ic->second;
                newcl->add(cv.getFrame(), cv.val);
            }
            tmpcll.insert(std::pair<const int, CtrlList*>(newcl->id(), newcl));
        }
        else
        {
            newcl = new CtrlList();
            *newcl = *cl;
            tmpcll.insert(std::pair<const int, CtrlList*>(newcl->id(), newcl));
        }
    }

    for (iCtrlList ci = _controller.begin(); ci != _controller.end(); ++ci)
        delete (*ci).second;

    _controller.clear();

    for (ciCtrlList icl = tmpcll.begin(); icl != tmpcll.end(); ++icl)
    {
        newcl = icl->second;
        _controller.insert(std::pair<const int, CtrlList*>(newcl->id(), newcl));
    }
}

//---------------------------------------------------------
//   setAutomationType
//---------------------------------------------------------

void AudioTrack::setAutomationType(AutomationType t)
{
    // Clear pressed and touched and rec event list.
    clearRecAutomation(true);

    // Now set the type.
    _automationType = t;
}

//---------------------------------------------------------
//   processAutomationEvents
//---------------------------------------------------------

void AudioTrack::processAutomationEvents()/*{{{*/
{
    if (_automationType != AUTO_TOUCH && _automationType != AUTO_WRITE)
        return;

    for (iCtrlList icl = _controller.begin(); icl != _controller.end(); ++icl)
    {
        CtrlList* cl = icl->second;
        int id = cl->id();

        // Remove old events from record region.
        if (_automationType == AUTO_WRITE)
        {
            int start = audio->getStartRecordPos().frame();
            int end = audio->getEndRecordPos().frame();
            iCtrl s = cl->lower_bound(start);
            iCtrl e = cl->lower_bound(end);

            // Erase old events only if there were recorded events.
            for (iCtrlRec icr = _recEvents.begin(); icr != _recEvents.end(); ++icr)
            {
                if (icr->id == id) // && icr->type == ARVT_VAL && icr->frame >= s->frame && icr->frame <= e->frame)
                {
                    cl->erase(s, e);
                    break;
                }
            }
        }
        else
        { // type AUTO_TOUCH
            for (iCtrlRec icr = _recEvents.begin(); icr != _recEvents.end(); ++icr)
            {
                // Don't bother looking for start, it's OK, just take the first one.
                // Needed for mousewheel and paging etc.
                //if (icr->id == id && icr->type == ARVT_START)
                if (icr->id == id)
                {
                    int start = icr->getFrame();

                    if (icr == _recEvents.end())
                    {
                        int end = audio->getEndRecordPos().frame();
                        iCtrl s = cl->lower_bound(start);
                        iCtrl e = cl->lower_bound(end);
                        cl->erase(s, e);
                        break;
                    }

                    iCtrlRec icrlast = icr;
                    ++icr;
                    for (;; ++icr)
                    {
                        if (icr == _recEvents.end())
                        {
                            int end = icrlast->getFrame();
                            iCtrl s = cl->lower_bound(start);
                            iCtrl e = cl->lower_bound(end);
                            cl->erase(s, e);
                            break;
                        }

                        if (icr->id == id && icr->type == ARVT_STOP)
                        {
                            int end = icr->getFrame();
                            // Erase everything up to, not including, this stop event's frame.
                            // Because an event was already stored directly when slider released.
                            if (end > start)
                                --end;

                            iCtrl s = cl->lower_bound(start);
                            iCtrl e = cl->lower_bound(end);

                            cl->erase(s, e);

                            break;
                        }

                        if (icr->id == id)
                            icrlast = icr;
                    }
                    if (icr == _recEvents.end())
                        break;
                }
            }
        }

        // Extract all recorded events for controller "id"
        //  from CtrlRecList and put into cl.
        for (iCtrlRec icr = _recEvents.begin(); icr != _recEvents.end(); ++icr)
        {
            if (icr->id == id && (icr->type == ARVT_VAL || icr->type == ARVT_START))
                cl->add(icr->getFrame(), icr->val);
        }
    }

    // Done with the recorded automation event list. Clear it.
    _recEvents.clear();

    // Try los without this, so that the user can remain in automation write mode
    //  after a stop.
    /*
    if (automationType() == AUTO_WRITE)
      {
          setAutomationType(AUTO_READ);
          song->update(SC_AUTOMATION);
      }
     */

}/*}}}*/

/*double AudioTrack::getAutomationEventForID(int id, int frame)//{{{
{
    // Erase old events only if there were recorded events.
    for (iCtrlRec icr = _recEvents.begin(); icr != _recEvents.end(); ++icr)
    {
        if (icr->id == id && icr->frame == frame) // && icr->type == ARVT_VAL && icr->frame >= s->frame && icr->frame <= e->frame)
        {
            return icr->val;
        }
    }

}//}}}*/

//---------------------------------------------------------
//   setControllerMode
//---------------------------------------------------------

void AudioTrack::setControllerMode(int ctlID, CtrlList::Mode m)
{
    ciCtrlList cl = _controller.find(ctlID);
    if (cl == _controller.end())
        return;

    cl->second->setMode(m);
}

//---------------------------------------------------------
//   clearControllerEvents
//---------------------------------------------------------

void AudioTrack::clearControllerEvents(int id)
{
    ciCtrlList icl = _controller.find(id);
    if (icl == _controller.end())
        return;

    CtrlList* cl = icl->second;
    cl->clear();
    return;
}

//---------------------------------------------------------
//   seekPrevACEvent
//---------------------------------------------------------

void AudioTrack::seekPrevACEvent(int id)
{
    ciCtrlList icl = _controller.find(id);
    if (icl == _controller.end())
        return;

    CtrlList* cl = icl->second;
    if (cl->empty())
        return;

    iCtrl s = cl->lower_bound(song->cPos().frame());
    if (s != cl->begin())
        --s;
    song->setPos(Song::CPOS, Pos(s->second.getFrame(), false), true, false, true);
    return;
}

//---------------------------------------------------------
//   seekNextACEvent
//---------------------------------------------------------

void AudioTrack::seekNextACEvent(int id)
{
    ciCtrlList icl = _controller.find(id);
    if (icl == _controller.end())
        return;

    CtrlList* cl = icl->second;
    if (cl->empty())
        return;

    iCtrl s = cl->upper_bound(song->cPos().frame());
    if (s == cl->end())
    {
        --s;
    }

    song->setPos(Song::CPOS, Pos(s->second.getFrame(), false), true, false, true);
    return;
}

//---------------------------------------------------------
//   eraseACEvent
//---------------------------------------------------------

void AudioTrack::eraseACEvent(int id, int frame)
{
    ciCtrlList icl = _controller.find(id);
    if (icl == _controller.end())
        return;

    CtrlList* cl = icl->second;
    if (cl->empty())
        return;

    iCtrl s = cl->find(frame);
    if (s != cl->end())
        cl->erase(s);
    return;
}

//---------------------------------------------------------
//   eraseRangeACEvents
//---------------------------------------------------------

void AudioTrack::eraseRangeACEvents(int id, int frame1, int frame2)
{
    ciCtrlList icl = _controller.find(id);
    if (icl == _controller.end())
        return;

    CtrlList* cl = icl->second;
    if (cl->empty())
        return;

    iCtrl s = cl->lower_bound(frame1);
    iCtrl e = cl->lower_bound(frame2);
    cl->erase(s, e);
    return;
}

//---------------------------------------------------------
//   addACEvent
//---------------------------------------------------------

void AudioTrack::addACEvent(int id, int frame, double val)
{
    ciCtrlList icl = _controller.find(id);
    if (icl == _controller.end())
        return;

    CtrlList* cl = icl->second;

    // Add will replace if found.
    cl->add(frame, val);
    return;
}

bool AudioTrack::volFromAutomation()
{
    ciCtrlList cl = _controller.find(AC_VOLUME);
    if (cl == _controller.end())
        return false;

    if (automation && automationType() != AUTO_OFF && _volumeEnCtrl && _volumeEn2Ctrl)
        return true;

    return false;
}

//---------------------------------------------------------
//   volume
//---------------------------------------------------------

double AudioTrack::volume() const/*{{{*/
{
    ciCtrlList cl = _controller.find(AC_VOLUME);
    if (cl == _controller.end())
        return 0.0;

    if (automation &&
            automationType() != AUTO_OFF && _volumeEnCtrl && _volumeEn2Ctrl)
        return cl->second->value(song->cPos().frame());
    else
        return cl->second->curVal();
}/*}}}*/

double AudioTrack::volume(unsigned frame) const/*{{{*/
{
    ciCtrlList cl = _controller.find(AC_VOLUME);
    if (cl == _controller.end())
        return 0.0;

    if (automation &&
            automationType() != AUTO_OFF && _volumeEnCtrl && _volumeEn2Ctrl)
        return cl->second->value(frame);
    else
        return cl->second->curVal();
}/*}}}*/

//---------------------------------------------------------
//   setVolume
//---------------------------------------------------------

void AudioTrack::setVolume(double val, bool monitor)
{
    iCtrlList cl = _controller.find(AC_VOLUME);
    if (cl == _controller.end())
    {
        printf("no volume controller %s %zd\n",
                name().toLatin1().constData(), _controller.size());
        return;
    }
    cl->second->setCurVal(val);
    if(!monitor)
    {
        //Call the monitor here if it was not called from the monitor
        midiMonitor->msgSendAudioOutputEvent((Track*)this, CTRL_VOLUME, val);
    }
}

bool AudioTrack::panFromAutomation()
{
    ciCtrlList cl = _controller.find(AC_PAN);
    if (cl == _controller.end())
        return false;

    if (automation && automationType() != AUTO_OFF && _panEnCtrl && _panEn2Ctrl)
        return true;

    return false;
}

//---------------------------------------------------------
//   pan
//---------------------------------------------------------

double AudioTrack::pan() const
{
    ciCtrlList cl = _controller.find(AC_PAN);
    if (cl == _controller.end())
        return 0.0;

    if (automation &&
            automationType() != AUTO_OFF && _panEnCtrl && _panEn2Ctrl)
        return cl->second->value(song->cPos().frame());
    else
        return cl->second->curVal();
}

//---------------------------------------------------------
//   setPan
//---------------------------------------------------------

void AudioTrack::setPan(double val, bool monitor)
{
    iCtrlList cl = _controller.find(AC_PAN);
    if (cl == _controller.end())
    {
        printf("no pan controller\n");
        return;
    }
    cl->second->setCurVal(val);
    if(!monitor)
    {
        //Call the monitor here if it was not called from the monitor
        midiMonitor->msgSendAudioOutputEvent((Track*)this, CTRL_PANPOT, val);
    }
}

void AudioTrack::recordAutomation(int n, double v)
{
    if (!automation)
        return;
    if (audio->isPlaying())
    {
        _recEvents.push_back(CtrlRecVal(song->cPos().frame(), n, v));
    }
    else
    {
        if (automationType() == AUTO_WRITE)
            _recEvents.push_back(CtrlRecVal(song->cPos().frame(), n, v));
        else
            if (automationType() == AUTO_TOUCH)
            // In touch mode and not playing. Send directly to controller list.
        {
            iCtrlList cl = _controller.find(n);
            if (cl == _controller.end())
                return;
            // Add will replace if found.
            cl->second->add(song->cPos().frame(), v);
        }
    }
}

void AudioTrack::startAutoRecord(int n, double v)
{
    if (!automation)
        return;
    if (audio->isPlaying())
    {
        if (automationType() == AUTO_TOUCH)
            _recEvents.push_back(CtrlRecVal(song->cPos().frame(), n, v, ARVT_START));
        else
            if (automationType() == AUTO_WRITE)
            _recEvents.push_back(CtrlRecVal(song->cPos().frame(), n, v));
    }
    else
    {
        if (automationType() == AUTO_TOUCH)
            // In touch mode and not playing. Send directly to controller list.
        {
            iCtrlList cl = _controller.find(n);
            if (cl == _controller.end())
                return;
            // Add will replace if found.
            cl->second->add(song->cPos().frame(), v);
        }
        else
            if (automationType() == AUTO_WRITE)
            _recEvents.push_back(CtrlRecVal(song->cPos().frame(), n, v));
    }
    //Trigger update of the gui here
    //song->update(SC_TRACK_MODIFIED|SC_VIEW_CHANGED);
}

void AudioTrack::stopAutoRecord(int n, double v)
{
    if (!automation)
        return;
    if (audio->isPlaying())
    {
        if (automationType() == AUTO_TOUCH)
        {
            audio->msgAddACEvent(this, n, song->cPos().frame(), v);
            _recEvents.push_back(CtrlRecVal(song->cPos().frame(), n, v, ARVT_STOP));
        }
    }
}

//---------------------------------------------------------
//   AudioTrack::writeProperties
//---------------------------------------------------------

void AudioTrack::writeProperties(int level, Xml& xml) const
{
    Track::writeProperties(level, xml);
    xml.intTag(level, "prefader", prefader());
    xml.intTag(level, "automation", int(automationType()));

    if (_wantsAutomation == false)
    {
        // _wantsAutomation is only set on fake midi automation tracks. on those, we don't need to save the plugins (synth) config
    }
    for (ciCtrlList icl = _controller.begin(); icl != _controller.end(); ++icl)
    {
        const CtrlList* cl = icl->second;

        QString s= QString("controller id=\"%1\" cur=\"%2\"").arg(cl->id()).arg(cl->curVal()).toAscii().constData();
        s += QString(" color=\"%1\" visible=\"%2\"").arg(cl->color().name()).arg(cl->isVisible());
        xml.tag(level++, s.toAscii().constData());
        int i = 0;
        for (ciCtrl ic = cl->begin(); ic != cl->end(); ++ic)
        {
            QString s("%1 %2, ");
            xml.nput(level, s.arg(ic->second.getFrame()).arg(ic->second.val).toAscii().constData());
            ++i;
            if (i >= 4)
            {
                xml.put(level, "");
                i = 0;
            }
        }
        if (i)
            xml.put(level, "");
        xml.etag(--level, "controller");
    }
}

//---------------------------------------------------------
//   AudioTrack::readProperties
//---------------------------------------------------------

bool AudioTrack::readProperties(Xml& xml, const QString& tag)
{
    if (tag == "prefader")
        _prefader = xml.parseInt();
    else if (tag == "automation")
        setAutomationType(AutomationType(xml.parseInt()));
    else if (tag == "controller")
    {
        CtrlList* l = new CtrlList();
        l->read(xml);

        // Since (until now) los wrote a 'zero' for plugin controller current value
        //  in the XML file, we can't use that value, now that plugin automation is added.
        // We must take the value from the plugin control value.
        // Otherwise we break all existing .los files with plugins, because the gui
        //  controls would all be set to zero.
        // But we will allow for the (unintended, useless) possibility of a controller
        //  with no matching plugin control.
        bool ctlfound = false;

        iCtrlList icl = _controller.find(l->id());
        if (icl == _controller.end())
            _controller.add(l);
        else
        {
            CtrlList* d = icl->second;
            for (iCtrl i = l->begin(); i != l->end(); ++i)
                d->add(i->second.getFrame(), i->second.val);

            if (!ctlfound)
                d->setCurVal(l->curVal());
            d->setColor(l->color());
            d->setVisible(l->isVisible());

            d->setDefault(l->getDefault());
            delete l;
            l = d;
        }
    }
    else
    {
        bool rv = Track::readProperties(xml, tag);
        return rv;
    }
    return false;
}

//---------------------------------------------------------
//   AudioInputHelper
//---------------------------------------------------------

AudioInputHelper::AudioInputHelper()
: AudioTrack(WAVE_INPUT_HELPER)
{
    // set Default for Input Ports:
    _mute = false;
    //setVolume(1.0);
    for (int i = 0; i < MAX_CHANNELS; ++i)
        jackPorts[i] = 0;
}


AudioInputHelper::AudioInputHelper(const AudioInputHelper& t, bool cloneParts)
: AudioTrack(t, cloneParts)
{
    for (int i = 0; i < MAX_CHANNELS; ++i)
        jackPorts[i] = t.jackPorts[i];
}

//---------------------------------------------------------
//   ~AudioInputHelper
//---------------------------------------------------------

AudioInputHelper::~AudioInputHelper()
{
    if (!checkAudioDevice()) return;
    for (int i = 0; i < _channels; ++i)
        if (jackPorts[i])
            audioDevice->unregisterPort(jackPorts[i]);
}

//---------------------------------------------------------
//   write
//---------------------------------------------------------

void AudioInputHelper::write(int level, Xml& xml) const
{
    xml.tag(level++, "AudioInput");
    AudioTrack::writeProperties(level, xml);
    xml.etag(--level, "AudioInput");
}

//---------------------------------------------------------
//   read
//---------------------------------------------------------

void AudioInputHelper::read(Xml& xml)
{
    for (;;)
    {
        Xml::Token token = xml.parse();
        const QString& tag = xml.s1();
        switch (token)
        {
            case Xml::Error:
            case Xml::End:
                return;
            case Xml::TagStart:
                if (AudioTrack::readProperties(xml, tag))
                    xml.unknown("AudioInput");
                break;
            case Xml::Attribut:
                break;
            case Xml::TagEnd:
                if (tag == "AudioInput")
                {
                    setName(name()); // allocate jack ports
                    return;
                }
            default:
                break;
        }
    }
}

//---------------------------------------------------------
//   AudioOutputHelper
//---------------------------------------------------------

AudioOutputHelper::AudioOutputHelper()
    : AudioTrack(WAVE_OUTPUT_HELPER)
{
    for (int i = 0; i < MAX_CHANNELS; ++i)
        jackPorts[i] = 0;
}

AudioOutputHelper::AudioOutputHelper(const AudioOutputHelper& t, bool cloneParts)
: AudioTrack(t, cloneParts)
{
    for (int i = 0; i < MAX_CHANNELS; ++i)
        jackPorts[i] = t.jackPorts[i];
    _nframes = t._nframes;
}

//---------------------------------------------------------
//   ~AudioOutputHelper
//---------------------------------------------------------

AudioOutputHelper::~AudioOutputHelper()
{
    if (!checkAudioDevice()) return;
    for (int i = 0; i < _channels; ++i)
        if (jackPorts[i])
            audioDevice->unregisterPort(jackPorts[i]);
}

//---------------------------------------------------------
//   write
//---------------------------------------------------------

void AudioOutputHelper::write(int level, Xml& xml) const
{
    xml.tag(level++, "AudioOutput");
    AudioTrack::writeProperties(level, xml);
    xml.etag(--level, "AudioOutput");
}

//---------------------------------------------------------
//   read
//---------------------------------------------------------

void AudioOutputHelper::read(Xml& xml)
{
    for (;;)
    {
        Xml::Token token = xml.parse();
        const QString& tag = xml.s1();
        switch (token)
        {
            case Xml::Error:
            case Xml::End:
                return;
            case Xml::TagStart:
                if (AudioTrack::readProperties(xml, tag))
                    xml.unknown("AudioOutput");
                break;
            case Xml::Attribut:
                break;
            case Xml::TagEnd:
                if (tag == "AudioOutput")
                {
                    setName(name()); // allocate jack ports
                    return;
                }
            default:
                break;
        }
    }
}

//---------------------------------------------------------
//   setRecordFlag1
//    gui part (executed in gui thread)
//---------------------------------------------------------

bool AudioTrack::setRecordFlag1(bool f, bool monitor)/*{{{*/
{
    if(!monitor)
    {
        //Call the monitor here if it was not called from the monitor
        midiMonitor->msgSendMidiOutputEvent((Track*)this, CTRL_RECORD, f ? 127 : 0);
    }
    if (f == _recordFlag)
        return true;
    if (f)
    {
        if (_recFile == 0)
        {
            // do nothing
            if (_recFile == 0 && song->record())
            {
                // this rec-enables a track if the global arm already was done
                // the standard case would be that rec-enable be done there
                prepareRecording();
            }
        }
    }
    else
    {
        if (_recFile)
        {
            // this file has not been processed and can be
            // deleted
            // We should only arrive here if going from a 'record-armed' state
            //  to a non record-armed state. Because otherwise after actually
            //  recording, the _recFile pointer is made into an event,
            //  then _recFile is made zero before this function is called.
            QString s = _recFile->path();
            // Added by Tim. p3.3.8
            delete _recFile;
            setRecFile(0);

            remove(s.toLatin1().constData());
            if (debugMsg)
                printf("AudioNode::setRecordFlag1: remove file %s if it exists\n", s.toLatin1().constData());
            //_recFile = 0;
        }
    }
    return true;
}/*}}}*/

//---------------------------------------------------------
// prepareRecording
// normally called from song->setRecord to defer creating
// wave files until LOS is globally rec-enabled
// also called from track->setRecordFlag (above)
// if global rec enable already was done
//---------------------------------------------------------
bool AudioTrack::prepareRecording()/*{{{*/
{
    if(debugMsg)
        printf("prepareRecording for track %s\n", _name.toLatin1().constData());

    if (_recFile == 0)
    {
         //
         //create soundfile for recording
         //
         char buffer[128];
         QFile fil;
         for (;;++recFileNumber)
         {
            sprintf(buffer, "%s/rec%d.wav",losProject.toLatin1().constData(),recFileNumber);
            fil.setFileName(QString(buffer));
            if (!fil.exists())
            break;
         }
         _recFile = new SndFile(QString(buffer));
         _recFile->setFormat(SF_FORMAT_WAV | SF_FORMAT_FLOAT, _channels, sampleRate);
    }
    if (debugMsg)
        printf("AudioNode::setRecordFlag1: init internal file %s\n", _recFile->path().toLatin1().constData());
    if (_recFile->openWrite())
    {
        QMessageBox::critical(NULL, "LOS write error.", "Error creating target wave file\n"
                "Check your configuration.");
        return false;

    }
    return true;
}/*}}}*/
