//=========================================================
//  LOS
//  Libre Octave Studio
//  $Id: song.cpp,v 1.59.2.52 2009/12/15 03:39:58 terminator356 Exp $
//
//  (C) Copyright 2000-2004 Werner Schweer (ws@seh.de)
//  (C) Copyright 2011-2012 The Open Octave Project <info@openoctave.org>
//=========================================================

#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#include <QAction>
#include <QDir>
#include <QMenu>
#include <QMessageBox>
#include <QPoint>
#include <QSignalMapper>
#include <QTextStream>
#include <QUndoStack>

#include "app.h"
#include "driver/jackmidi.h"
#include "driver/alsamidi.h"
#include "song.h"
#include "track.h"
#include "undo.h"
#include "globals.h"
#include "event.h"
#include "marker/marker.h"
#include "audio.h"
#include "mididev.h"
#include "midiport.h"
#include "midiseq.h"
#include "driver/audiodev.h"
#include "gconfig.h"
#include "midictrl.h"
#include "menutitleitem.h"
#include "midi.h"
#include "sig.h"
#include <sys/wait.h>
#include "trackview.h"
#include "mpevent.h"
#include "midimonitor.h"
#include "traverso_shared/OOMCommand.h"
#include "traverso_shared/TConfig.h"
#include "CreateTrackDialog.h"
#include "minstrument.h"

extern void clearMidiTransforms();
extern void clearMidiInputTransforms();
Song* song;

/*
//---------------------------------------------------------
//   RoutingMenuItem
//---------------------------------------------------------

class RoutingMenuItem : public QCustomMenuItem
{
      Route route;
      //virtual QSize sizeHint() { return QSize(80, h); }
      virtual void paint(QPainter* p, const QColorGroup&, bool, bool, int x, int y, int w, int h)
      {
        p->fillRect(x, y, w, h, QBrush(lightGray));
        p->drawText(x, y, w, h, AlignCenter, route.name());
      }

   public:
      RoutingMenuItem(const Route& r) : route(r) { }
};
 */

//---------------------------------------------------------
//   Song
//---------------------------------------------------------

Song::Song(QUndoStack* stack, const char* name)
: QObject(0)
{
    setObjectName(name);
    _arrangerRaster = 0; // Set to measure, the same as Arranger intial value. Arranger snap combo will set this.
    noteFifoSize = 0;
    noteFifoWindex = 0;
    noteFifoRindex = 0;
    undoList = new UndoList;
    redoList = new UndoList;
    m_undoStack = stack; //new QUndoStack(this);
    _markerList = new MarkerList;
    _globalPitchShift = 0;
    jackErrorBox = 0;
    viewselected = false;
    hasSelectedParts = false;
    invalid = false;
    _replay = false;
    _replayPos = 0;
    //Create the AutoView
    TrackView* wv = new TrackView();
    wv->setViewName(tr("Working View"));
    wv->setSelected(false);
    m_autoTrackViewIndex.append(wv->id());
    _autotviews[wv->id()] = wv;
    m_workingViewId = wv->id();
    //TODO:Set working view ID for later use
    TrackView* iv = new TrackView();
    iv->setViewName(tr("Inputs  View"));
    iv->setSelected(false);
    _autotviews[iv->id()] = iv;
    m_autoTrackViewIndex.append(iv->id());
    m_inputViewId = iv->id();
    //TODO:Set view ID for later use
    TrackView* ov = new TrackView();
    ov->setViewName(tr("Outputs View"));
    ov->setSelected(false);
    _autotviews[ov->id()] = ov;
    m_autoTrackViewIndex.append(ov->id());
    m_outputViewId = ov->id();
    //TODO:Set view ID for later use
    TrackView* cv = new TrackView();
    cv->setViewName(tr("Comment View"));
    cv->setSelected(false);
    _autotviews[cv->id()] = cv;
    m_autoTrackViewIndex.append(cv->id());
    m_commentViewId = cv->id();
    //TODO:Set view ID for later use
    QHash<int, QString> hash;

    QStringList map;
    map << "C-2" << "C#-2" << "D-2" << "D#-2" << "E-2" << "F-2" << "F#-2" << "G-2" << "G#-2" << "A-2" << "A#-2" << "B-2" << "C-1" ;
    map << "C#-1" << "D-1" << "D#-1" << "E-1" << "F-1" << "F#-1" << "G-1" << "G#-1" << "A-1" << "A#-1" << "B-1" << "C0" << "C#0" << "D0" << "D#0" << "E0" << "F0" << "F#0";
    map << "G0" << "G#0" << "A0" << "A#0" << "B0" << "C1" << "C#1" << "D1" << "D#1" << "E1" << "F1" << "F#1" << "G1" << "G#1" << "A1" << "A#1";
    map << "B1" << "C2" << "C#2" << "D2" << "D#2" << "E2" << "F2" << "F#2" << "G2" << "G#2" << "A2" << "A#2" << "B2" << "C3" << "C#3" << "D3";
    map << "D#3" << "E3" << "F3" << "F#3" << "G3" << "G#3" << "A3" << "A#3" << "B3" << "C4" << "C#4" << "D4" << "D#4" << "E4" << "F4" << "F#4";
    map << "G4" << "G#4" << "A4" << "A#4" << "B4" << "C5" << "C#5" << "D5" << "D#5" << "E5" << "F5" << "F#5" << "G5" << "G#5" << "A5" << "A#5";
    map << "B5" << "C6" << "C#6" << "D6" << "D#6" << "E6" << "F6" << "F#6" << "G6" << "G#6" << "A6" << "A#6" << "B6" << "C7" << "C#7" << "D7";
    map << "D#7" << "E7" << "F7" << "F#7" << "G7" << "G#7" << "A7" << "A#7" << "B7" << "C8" << "C#8" << "D8" << "D#8" << "E8" << "F8" << "F#8" << "G8";
    for(int i = 0; i < 128; ++i)
    {
        m_midiKeys[i] = map.at(i);
    }

    clear(false);
}

//---------------------------------------------------------
//   Song
//---------------------------------------------------------

Song::~Song()
{
    delete undoList;
    delete redoList;
    delete _markerList;
}

//---------------------------------------------------------
//   putEvent
//---------------------------------------------------------

void Song::putEvent(int pv)
{
    if (noteFifoSize < REC_NOTE_FIFO_SIZE)
    {
        recNoteFifo[noteFifoWindex] = pv;
        noteFifoWindex = (noteFifoWindex + 1) % REC_NOTE_FIFO_SIZE;
        ++noteFifoSize;
    }
}

//---------------------------------------------------------
//   setTempo
//    public slot
//---------------------------------------------------------

void Song::setTempo(int newTempo)
{
    audio->msgSetTempo(pos[0].tick(), newTempo, true);
}

//---------------------------------------------------------
//   setSig
//    called from transport window
//---------------------------------------------------------

void Song::setSig(int z, int n)
{
    if (_masterFlag)
    {
        audio->msgAddSig(pos[0].tick(), z, n);
    }
}

void Song::setSig(const TimeSignature& sig)
{
    if (_masterFlag)
    {
        audio->msgAddSig(pos[0].tick(), sig.z, sig.n);
    }
}

//---------------------------------------------------------
//    addNewTrack
//    Called from GUI context
//    Besides normal track types, n includes synth menu ids from populateAddTrack()
//---------------------------------------------------------

MidiTrack* Song::addNewTrack()
{
    CreateTrackDialog *ctdialog = new CreateTrackDialog(-1, los);
    connect(ctdialog, SIGNAL(trackAdded(qint64)), this, SLOT(newTrackAdded(qint64)));
    ctdialog->exec();
    return 0;
}


void Song::newTrackAdded(qint64 id)
{
    Track* t = findTrackById(id);
    if(t)
    {
        updateTrackViews();
        update(SC_SELECTION);
    }
}

//---------------------------------------------------------
//    addTrack
//    called from GUI context
//---------------------------------------------------------

MidiTrack* Song::addTrack(bool doUndo)/*{{{*/
{
    MidiTrack* track = nullptr;

    {
        track = new MidiTrack();

        if(config.partColorNames[lastTrackPartColorIndex].contains("menu:", Qt::CaseSensitive))
            lastTrackPartColorIndex ++;

        track->setDefaultPartColor(lastTrackPartColorIndex);
        lastTrackPartColorIndex ++;

        if(lastTrackPartColorIndex == NUM_PARTCOLORS)
            lastTrackPartColorIndex = 1;
    }

    track->setDefaultName();
    track->setHeight(DEFAULT_TRACKHEIGHT);
    insertTrack1(track, -1);
    msgInsertTrack(track, -1, doUndo);
    midiMonitor->msgAddMonitoredTrack(track);

    {
        MidiTrack* mt = (MidiTrack*) track;
        int c, cbi, ch;
        bool defOutFound = false; /// TODO: Remove this when multiple out routes supported.
        for (int i = 0; i < kMaxMidiPorts; ++i)
        {
            MidiPort* mp = &midiPorts[i];

            c = mp->defaultInChannels();
            if (c)
            {
                audio->msgAddRoute(Route(i, c), Route(track, c));
                updateFlags |= SC_ROUTE;
            }

            if (!defOutFound)
            {
                c = mp->defaultOutChannels();
                if (c)
                {
                    for (ch = 0; ch < kMaxMidiChannels; ++ch)
                    {
                        cbi = 1 << ch;
                        if (c & cbi)
                        {
                            defOutFound = true;
                            mt->setOutPort(i);
                            mt->setOutChannel(ch);
                            updateFlags |= SC_ROUTE;
                            break;
                        }
                    }
                }
            }
        }
    }

    audio->msgUpdateSoloStates();
    //updateTrackViews();
    return track;
}/*}}}*/

MidiTrack* Song::addTrackByName(QString name, int pos, bool doUndo)/*{{{*/
{
    MidiTrack* track = 0;

    {
        track = new MidiTrack();

        if(config.partColorNames[lastTrackPartColorIndex].contains("menu:", Qt::CaseSensitive))
            lastTrackPartColorIndex ++;

        track->setDefaultPartColor(lastTrackPartColorIndex);
        lastTrackPartColorIndex ++;

        if(lastTrackPartColorIndex == NUM_PARTCOLORS)
            lastTrackPartColorIndex = 1;
    }

    track->setName(track->getValidName(name));
    track->setHeight(DEFAULT_TRACKHEIGHT);
    insertTrack1(track, pos);
    msgInsertTrack(track, pos, doUndo);
    midiMonitor->msgAddMonitoredTrack(track);

    {
        MidiTrack* mt = (MidiTrack*) track;
        int c, cbi, ch;
        bool defOutFound = false; /// TODO: Remove this when multiple out routes supported.
        for (int i = 0; i < kMaxMidiPorts; ++i)
        {
            MidiPort* mp = &midiPorts[i];

            c = mp->defaultInChannels();
            if (c)
            {
                audio->msgAddRoute(Route(i, c), Route(track, c));
                updateFlags |= SC_ROUTE;
            }

            if (!defOutFound) ///
            {
                c = mp->defaultOutChannels();
                if (c)
                {

                    /// TODO: Switch when multiple out routes supported.
#if 0
                    audio->msgAddRoute(Route(track, c), Route(i, c));
                    updateFlags |= SC_ROUTE;
#else
                    for (ch = 0; ch < kMaxMidiChannels; ++ch)
                    {
                        cbi = 1 << ch;
                        if (c & cbi)
                        {
                            defOutFound = true;
                            mt->setOutPort(i);
                            mt->setOutChannel(ch);
                            updateFlags |= SC_ROUTE;
                            break;
                        }
                    }
#endif
                }
            }
        }
    }

    audio->msgUpdateSoloStates();
    //updateTrackViews();
    return track;
}/*}}}*/

//---------------------------------------------------------
//   cmdRemoveTrack
//---------------------------------------------------------

void Song::cmdRemoveTrack(MidiTrack* track)
{
    int idx = _tracks.index(track);
    undoOp(UndoOp::DeleteTrack, idx, track);
    removeTrackRealtime(track);
    updateFlags |= SC_TRACK_REMOVED;
}

//---------------------------------------------------------
//   removeMarkedTracks
//---------------------------------------------------------

void Song::removeMarkedTracks()
{
    bool loop;
    do
    {
        loop = false;
        for (iMidiTrack t = _tracks.begin(); t != _tracks.end(); ++t)
        {
            if ((*t)->selected())
            {
                removeTrackRealtime(*t);
                loop = true;
                break;
            }
        }
    } while (loop);
}

//---------------------------------------------------------
//   deselectTracks
//---------------------------------------------------------

void Song::deselectTracks()
{
    for (iMidiTrack t = _tracks.begin(); t != _tracks.end(); ++t)
        (*t)->setSelected(false);
}

void Song::deselectAllParts()
{
    for(iMidiTrack t = _tracks.begin(); t != _tracks.end(); ++t)
        (*t)->deselectParts();
    hasSelectedParts = false;
    update(SC_SELECTION);
}

void Song::disarmAllTracks()
{
    //printf("Song::disarmAllTracks()\n");
    if(viewselected)
    {
        unsigned int noop = -1;
        for(iMidiTrack t = _tracks.begin(); t != _tracks.end(); ++t)
        {
            if(_viewtracks.index((*t)) == noop)
            {
                (*t)->setRecordFlag1(false);
                (*t)->setRecordFlag2(false);
                (*t)->setSelected(false);
            }
        }
    }
}

//---------------------------------------------------------
//   changeTrack
//    oldTrack - copy of the original track befor modification
//    newTrack - modified original track
//---------------------------------------------------------

void Song::changeTrack(MidiTrack* oldTrack, MidiTrack* newTrack)
{
    oldTrack->setSelected(false); //??
    int idx = _tracks.index(newTrack);

    undoOp(UndoOp::ModifyTrack, idx, oldTrack, newTrack);
    updateFlags |= SC_TRACK_MODIFIED;
}

//---------------------------------------------------------
//  addEvent
//    return true if event was added
//---------------------------------------------------------

bool Song::addEvent(Event& event, Part* part)
{
    // Return false if the event is already found.
    if (part->events()->find(event) != part->events()->end())
    {
        // This can be normal for some (redundant) operations.
        if (debugMsg)
            printf("Song::addEvent event already found in part:%s size:%zd\n", part->name().toLatin1().constData(), part->events()->size());
        return false;
    }

    part->events()->add(event);
    return true;
}

//---------------------------------------------------------
//   changeEvent
//---------------------------------------------------------

void Song::changeEvent(Event& oldEvent, Event& newEvent, Part* part)
{
    iEvent i = part->events()->find(oldEvent);

    if (i == part->events()->end())
    {
        // This can be normal for some (redundant) operations.
        if (debugMsg)
            printf("Song::changeEvent event not found in part:%s size:%zd\n", part->name().toLatin1().constData(), part->events()->size());
    }
    else
        part->events()->erase(i);

    part->events()->add(newEvent);
}

//---------------------------------------------------------
//   deleteEvent
//---------------------------------------------------------

void Song::deleteEvent(Event& event, Part* part)
{
    iEvent ev = part->events()->find(event);
    if (ev == part->events()->end())
    {
        // This can be normal for some (redundant) operations.
        if (debugMsg)
            printf("Song::deleteEvent event not found in part:%s size:%zd\n", part->name().toLatin1().constData(), part->events()->size());
        return;
    }
    part->events()->erase(ev);
}

//---------------------------------------------------------
//   cmdAddRecordedEvents
//    add recorded Events into part
//---------------------------------------------------------

void Song::cmdAddRecordedEvents(MidiTrack* mt, EventList* events, unsigned startTick)
{
    if (events->empty())
    {
        if (debugMsg)
            printf("no events recorded\n");
        return;
    }
    iEvent s;
    iEvent e;
    unsigned endTick;

    // Changed by Tim. p3.3.8

    //if (punchin())
    if ((audio->loopCount() > 0 && startTick > lPos().tick()) || (punchin() && startTick < lPos().tick()))
    {
        startTick = lpos();
        s = events->lower_bound(startTick);
    }
    else
    {
        s = events->begin();
        //            startTick = s->first;
    }

    // Changed by Tim. p3.3.8

    //if (punchout())
    //{
    //      endTick = rpos();
    //      e = events->lower_bound(endTick);
    //}
    //else
    //{
    // search for last noteOff:
    endTick = 0;
    for (iEvent i = events->begin(); i != events->end(); ++i)
    {
        Event ev = i->second;
        unsigned l = ev.endTick();
        if (l > endTick)
            endTick = l;
    }
    //      e = events->end();
    //}
    if ((audio->loopCount() > 0) || (punchout() && endTick > rPos().tick()))
    {
        endTick = rpos();
        e = events->lower_bound(endTick);
    }
    else
        e = events->end();

    if (startTick > endTick)
    {
        if (debugMsg)
            printf("no events in record area\n");
        return;
    }

    //---------------------------------------------------
    //    if startTick points into a part,
    //          record to that part
    //    else
    //          create new part
    //---------------------------------------------------

    PartList* pl = mt->parts();
    MidiPart* part = 0;
    iPart ip;
    for (ip = pl->begin(); ip != pl->end(); ++ip)
    {
        part = (MidiPart*) (ip->second);
        unsigned partStart = part->tick();
        unsigned partEnd = part->endTick();
        if (startTick >= partStart && startTick < partEnd)
            break;
    }
    if (ip == pl->end())
    {
        if (debugMsg)
            printf("create new part for recorded events\n");
        // create new part
        part = new MidiPart(mt);

        // Changed by Tim. p3.3.8

        // Honour the Arranger snap settings. (Set to bar by default).
        //startTick = roundDownBar(startTick);
        //endTick   = roundUpBar(endTick);
        // Round the start down using the Arranger part snap raster value.
        startTick = sigmap.raster1(startTick, arrangerRaster());
        // Round the end up using the Arranger part snap raster value.
        endTick = sigmap.raster2(endTick, arrangerRaster());

        part->setTick(startTick);
        part->setLenTick(endTick - startTick);
        part->setName(mt->name());
        // copy events
        for (iEvent i = s; i != e; ++i)
        {
            Event old = i->second;
            Event event = old.clone();
            event.setTick(old.tick() - startTick);
            // addEvent also adds port controller values. So does msgAddPart, below. Let msgAddPart handle them.
            //addEvent(event, part);
            if (part->events()->find(event) == part->events()->end())
                part->events()->add(event);
        }
        audio->msgAddPart(part);
        updateFlags |= SC_PART_INSERTED;
        return;
    }

    updateFlags |= SC_EVENT_INSERTED;

    unsigned partTick = part->tick();
    if (endTick > part->endTick())
    {
        // Determine new part length...
        endTick = 0;
        for (iEvent i = s; i != e; ++i)
        {
            Event event = i->second;
            unsigned tick = event.tick() - partTick + event.lenTick();
            if (endTick < tick)
                endTick = tick;
        }
        // Added by Tim. p3.3.8

        // Round the end up (again) using the Arranger part snap raster value.
        endTick = sigmap.raster2(endTick, arrangerRaster());

        // Remove all of the part's port controller values. Indicate do not do clone parts.
        removePortCtrlEvents(part, false);
        // Clone the part. This doesn't increment aref count, and doesn't chain clones.
        // It also gives the new part a new serial number, but it is
        //  overwritten with the old one by Song::changePart(), below.
        Part* newPart = part->clone();
        // Set the new part's length.
        newPart->setLenTick(endTick);
        // Change the part.
        changePart(part, newPart);
        // Manually adjust reference counts.
        part->events()->incARef(-1);
        newPart->events()->incARef(1);
        // Replace the part in the clone chain with the new part.
        replaceClone(part, newPart);
        // Now add all of the new part's port controller values. Indicate do not do clone parts.
        addPortCtrlEvents(newPart, false);
        // Create an undo op. Indicate do port controller values but not clone parts.
        undoOp(UndoOp::ModifyPart, part, newPart, true, false);
        updateFlags |= SC_PART_MODIFIED;

        if (_recMode == REC_REPLACE)
        {
            iEvent si = newPart->events()->lower_bound(startTick - newPart->tick());
            iEvent ei = newPart->events()->lower_bound(newPart->endTick() - newPart->tick());
            for (iEvent i = si; i != ei; ++i)
            {
                Event event = i->second;
                // Create an undo op. Indicate do port controller values and clone parts.
                undoOp(UndoOp::DeleteEvent, event, newPart, true, true);
                // Remove the event from the new part's port controller values, and do all clone parts.
                removePortCtrlEvents(event, newPart, true);
            }
            newPart->events()->erase(si, ei);
        }

        for (iEvent i = s; i != e; ++i)
        {
            Event event = i->second;
            event.setTick(event.tick() - partTick);
            Event e;
            // Create an undo op. Indicate do port controller values and clone parts.
            undoOp(UndoOp::AddEvent, e, event, newPart, true, true);

            if (newPart->events()->find(event) == newPart->events()->end())
                newPart->events()->add(event);

            // Add the event to the new part's port controller values, and do all clone parts.
            addPortCtrlEvents(event, newPart, true);
        }
    }
    else
    {
        if (_recMode == REC_REPLACE)
        {
            iEvent si = part->events()->lower_bound(startTick - part->tick());
            iEvent ei = part->events()->lower_bound(endTick - part->tick());

            for (iEvent i = si; i != ei; ++i)
            {
                Event event = i->second;
                // Create an undo op. Indicate that controller values and clone parts were handled.
                undoOp(UndoOp::DeleteEvent, event, part, true, true);
                // Remove the event from the part's port controller values, and do all clone parts.
                removePortCtrlEvents(event, part, true);
            }
            part->events()->erase(si, ei);
        }
        for (iEvent i = s; i != e; ++i)
        {
            Event event = i->second;
            int tick = event.tick() - partTick;
            event.setTick(tick);

            // Create an undo op. Indicate that controller values and clone parts were handled.
            undoOp(UndoOp::AddEvent, event, part, true, true);

            if (part->events()->find(event) == part->events()->end())
                part->events()->add(event);

            // Add the event to the part's port controller values, and do all clone parts.
            addPortCtrlEvents(event, part, true);
        }
    }
}

//---------------------------------------------------------
//   findTrack
//---------------------------------------------------------

MidiTrack* Song::findTrack(const Part* part) const
{
    for (ciMidiTrack it = _tracks.begin(); it != _tracks.end(); ++it)
    {
        MidiTrack* track = dynamic_cast<MidiTrack*> (*it);
        if (track == 0)
            continue;
        PartList* pl = track->parts();
        for (iPart p = pl->begin(); p != pl->end(); ++p)
        {
            if (part == p->second)
                return track;
        }
    }
    return 0;
}

//---------------------------------------------------------
//   findTrack
//    find track by name
//---------------------------------------------------------

MidiTrack* Song::findTrack(const QString& name) const
{
    for (ciMidiTrack i = _tracks.begin(); i != _tracks.end(); ++i)
    {
        if ((*i)->name() == name)
            return *i;
    }
    return 0;
}

//---------------------------------------------------------
//   findTrackById
//    find track by id
//---------------------------------------------------------

MidiTrack* Song::findTrackById(qint64 id) const
{
    for (ciMidiTrack i = _tracks.begin(); i != _tracks.end(); ++i)
    {
        if ((*i)->id() == id)
            return *i;
    }
    return 0;
}

//---------------------------------------------------------
// setReplay
// set transport audition flag
//---------------------------------------------------------

void Song::setReplay(bool t)
{
    _replay = t;
    if(t)
    {
        _replayPos = song->cpos();
        emit replayChanged(_replay, _replayPos);
    }
}

void Song::updateReplayPos()
{
    _replayPos = song->cpos();
    emit replayChanged(_replay, _replayPos);
}

//---------------------------------------------------------
//   setLoop
//    set transport loop flag
//---------------------------------------------------------

void Song::setLoop(bool f)
{
    if (loopFlag != f)
    {
        loopFlag = f;
        loopAction->setChecked(loopFlag);
        emit loopChanged(loopFlag);
    }
}

//---------------------------------------------------------
//   clearTrackRec
//---------------------------------------------------------

void Song::clearTrackRec()
{
    //printf("Song::clearTrackRec()\n");
    for (iMidiTrack it = tracks()->begin(); it != tracks()->end(); ++it)
        setRecordFlag(*it, false);
}

//---------------------------------------------------------
//   setRecord
//---------------------------------------------------------

void Song::setRecord(bool f, bool autoRecEnable)
{
    if(debugMsg)
        printf("Song::setRecord recordflag =%d f(record state)=%d autoRecEnable=%d\n", recordFlag, f, autoRecEnable);
    if (f && losProject == losProjectInitPath)
    { // check that there is a project stored before commencing
        // no project, we need to create one.
        if (!los->saveAs())
            return; // could not store project, won't enable record
    }
    if (recordFlag != f)
    {
        if (f && autoRecEnable)
        {
            bool alreadyRecEnabled = false;
            MidiTrack *selectedTrack = 0;
            // loop through list and check if any track is rec enabled
            // if not then rec enable the selected track

            if (!alreadyRecEnabled)
            {
                MidiTrackList* mtl = midis();
                for (iMidiTrack it = mtl->begin(); it != mtl->end(); ++it)
                {
                    if ((*it)->recordFlag())
                    {
                        alreadyRecEnabled = true;
                        break;
                    }
                    if ((*it)->selected())
                        selectedTrack = (*it);
                }
            }
            if (!alreadyRecEnabled && selectedTrack)
            {
                setRecordFlag(selectedTrack, true);
            }
            else if (alreadyRecEnabled)
            {
                // do nothing
            }
            else
            {
                // if there are no tracks, do not enable record
                if (!midis()->size())
                {
                    printf("No track to select, won't enable record\n");
                    f = false;
                }
            }

#if 0
            // check for midi devices suitable for recording
            bool portFound = false;
            for (int i = 0; i < kMaxMidiPorts; ++i)
            {
                MidiDevice* dev = midiPorts[i].device();
                if (dev && (dev->rwFlags() & 0x2))
                    portFound = true;
            }
            if (!portFound)
            {
                QMessageBox::critical(qApp->mainWidget(), "LOS: Record",
                        "There are no midi devices configured for recording");
                f = false;
            }
#endif
        }
        else
        {
        }
        if (audio->isPlaying() && f)
            f = false;
        recordFlag = f;
        recordAction->setChecked(recordFlag);
        emit recordChanged(recordFlag);
    }
}

//---------------------------------------------------------
//   setPunchin
//    set punchin flag
//---------------------------------------------------------

void Song::setPunchin(bool f)
{
    if (punchinFlag != f)
    {
        punchinFlag = f;
        punchinAction->setChecked(punchinFlag);
        emit punchinChanged(punchinFlag);
    }
}

//---------------------------------------------------------
//   setPunchout
//    set punchout flag
//---------------------------------------------------------

void Song::setPunchout(bool f)
{
    if (punchoutFlag != f)
    {
        punchoutFlag = f;
        punchoutAction->setChecked(punchoutFlag);
        emit punchoutChanged(punchoutFlag);
    }
}

//---------------------------------------------------------
//   setQuantize
//---------------------------------------------------------

void Song::setQuantize(bool val)
{
    if (_quantize != val)
    {
        _quantize = val;
        emit quantizeChanged(_quantize);
    }
}

//---------------------------------------------------------
//   setMasterFlag
//---------------------------------------------------------

void Song::setMasterFlag(bool val)
{
    _masterFlag = val;
    if (tempomap.setMasterFlag(cpos(), val))
    {
        if(!invalid)
            emit songChanged(SC_MASTER);
    }
    masterEnableAction->blockSignals(true);
    masterEnableAction->setChecked(song->masterFlag());
    masterEnableAction->blockSignals(false);
}

//---------------------------------------------------------
//   setPlay
//    set transport play flag
//---------------------------------------------------------

void Song::setPlay(bool f)
{
    // only allow the user to set the button "on"
    if (!f)
        playAction->setChecked(true);
    else
    {
        audio->msgPlay(true);
    }
    emit playChanged(f); // signal transport window
}

void Song::setStop(bool f)
{
    // only allow the user to set the button "on"
    if (!f)
        stopAction->setChecked(true);
    else
    {
        audio->msgPlay(false);
        if(_replay)
        {
            Pos p(_replayPos, true);
            setPos(0, p, true, true, true);
        }
        //emit playbackStateChanged(false);
    }
}

void Song::setStopPlay(bool f)
{
    playAction->blockSignals(true);
    stopAction->blockSignals(true);

    emit playChanged(f); // signal transport window

    playAction->setChecked(f);
    stopAction->setChecked(!f);

    stopAction->blockSignals(false);
    playAction->blockSignals(false);
}

//---------------------------------------------------------
//   swapTracks
//---------------------------------------------------------

void Song::swapTracks(int i1, int i2)
{
    undoOp(UndoOp::SwapTrack, i1, i2);
    //printf("Song::swapTracks(int %d, int %d)\n", i1, i2);
    MidiTrack* track = _viewtracks[i1];
    _viewtracks[i1] = _viewtracks[i2];
    _viewtracks[i2] = track;

    if(!viewselected)
    {
        MidiTrack *t = _artracks[i1];
        _artracks[i1] = _artracks[i2];
        _artracks[i2] = t;
    }
    else
    {
        MidiTrack *t = _tracks[i1];
        _tracks[i1] = _tracks[i2];
        _tracks[i2] = t;
    }
}

//---------------------------------------------------------
//   setPos
//   song->setPos(Song::CPOS, pos, true, true, true);
//---------------------------------------------------------

void Song::setPos(int idx, const Pos& val, bool sig,
        bool isSeek, bool adjustScrollbar)
{
    //      printf("setPos %d sig=%d,seek=%d,scroll=%d  ",
    //         idx, sig, isSeek, adjustScrollbar);
    //      val.dump(0);
    //      printf("\n");

    // p3.3.23
    //printf("Song::setPos before audio->msgSeek idx:%d isSeek:%d frame:%d\n", idx, isSeek, val.frame());
    if (pos[idx] == val)
        return;
    if (idx == CPOS)
    {
        _vcpos = val;
        if (isSeek)
        {
            audio->msgSeek(val);
            // p3.3.23
            //printf("Song::setPos after audio->msgSeek idx:%d isSeek:%d frame:%d\n", idx, isSeek, val.frame());
            return;
        }
    }
    pos[idx] = val;
    bool swap = pos[LPOS] > pos[RPOS];
    if (swap)
    { // swap lpos/rpos if lpos > rpos
        Pos tmp = pos[LPOS];
        pos[LPOS] = pos[RPOS];
        pos[RPOS] = tmp;
    }
    if (sig)
    {
        if (swap)
        {
            emit posChanged(LPOS, pos[LPOS].tick(), adjustScrollbar);
            emit posChanged(RPOS, pos[RPOS].tick(), adjustScrollbar);
            if (idx != LPOS && idx != RPOS)
                emit posChanged(idx, pos[idx].tick(), adjustScrollbar);
        }
        else
            emit posChanged(idx, pos[idx].tick(), adjustScrollbar);
    }

    if (idx == CPOS)
    {
        iMarker i1 = _markerList->begin();
        iMarker i2 = i1;
        bool currentChanged = false;
        for (; i1 != _markerList->end(); ++i1)
        {
            ++i2;
            if (val.tick() >= i1->first && (i2 == _markerList->end() || val.tick() < i2->first))
            {
                if (i1->second.current())
                    return;
                i1->second.setCurrent(true);
                if (currentChanged)
                {
                    emit markerChanged(MARKER_CUR);
                    return;
                }
                ++i1;
                for (; i1 != _markerList->end(); ++i1)
                {
                    if (i1->second.current())
                        i1->second.setCurrent(false);
                }
                emit markerChanged(MARKER_CUR);
                return;
            }
            else
            {
                if (i1->second.current())
                {
                    currentChanged = true;
                    i1->second.setCurrent(false);
                }
            }
        }
        if (currentChanged)
            emit markerChanged(MARKER_CUR);
    }
}

//---------------------------------------------------------
//   forward
//---------------------------------------------------------

void Song::forward()
{
    unsigned newPos = pos[0].tick() + config.division;
    audio->msgSeek(Pos(newPos, true));
}

//---------------------------------------------------------
//   rewind
//---------------------------------------------------------

void Song::rewind()
{
    unsigned newPos;
    if (unsigned(config.division) > pos[0].tick())
        newPos = 0;
    else
        newPos = pos[0].tick() - config.division;
    audio->msgSeek(Pos(newPos, true));
}

//---------------------------------------------------------
//   rewindStart
//---------------------------------------------------------

void Song::rewindStart()
{
    // Added by T356
    //audio->msgIdle(true);

    audio->msgSeek(Pos(0, true));

    // Added by T356
    //audio->msgIdle(false);
}

//---------------------------------------------------------
//   update
//---------------------------------------------------------

void Song::update(int flags)
{
    static int level = 0; // DEBUG
    if (level)
    {
        printf("Song::update %08x, level %d\n", flags, level);
        return;
    }
    ++level;
    if(flags & (SC_TRACK_REMOVED | SC_TRACK_INSERTED/* | SC_TRACK_MODIFIED*/))
    {
        //printf("Song::update firing updateTrackViews\n");
        updateTrackViews();
    }
    /*if(flags & SC_VIEW_CHANGED)
    {
        emit arrangerViewChanged();
    }*/
    if(!invalid)
        emit songChanged(flags);
    --level;
}

//---------------------------------------------------------
//   updatePos
//---------------------------------------------------------

void Song::updatePos()
{
    emit posChanged(0, pos[0].tick(), false);
    emit posChanged(1, pos[1].tick(), false);
    emit posChanged(2, pos[2].tick(), false);
}

//---------------------------------------------------------
//   setChannelMute
//    mute all midi tracks associated with channel
//---------------------------------------------------------

void Song::setChannelMute(int channel, bool val)
{
    for (iMidiTrack i = _tracks.begin(); i != _tracks.end(); ++i)
    {
        MidiTrack* track = dynamic_cast<MidiTrack*> (*i);
        if (track == 0)
            continue;
        if (track->outChannel() == channel)
            track->setMute(val);
    }
    if(!invalid)
        emit songChanged(SC_MUTE);
}

//---------------------------------------------------------
//   len
//---------------------------------------------------------

void Song::initLen()
{
    _len = sigmap.bar2tick(264, 0, 0); // default song len
    for (iMidiTrack t = _tracks.begin(); t != _tracks.end(); ++t)
    {
        MidiTrack* track = (*t);
        PartList* parts = track->parts();
        for (iPart p = parts->begin(); p != parts->end(); ++p)
        {
            unsigned last = p->second->tick() + p->second->lenTick();
            if (last > _len)
                _len = last;
        }
    }
    _len = roundUpBar(_len);
}

//---------------------------------------------------------
//   tempoChanged
//---------------------------------------------------------

void Song::tempoChanged()
{
    if(!invalid)
        emit songChanged(SC_TEMPO);
}

//---------------------------------------------------------
//   roundUpBar
//---------------------------------------------------------

int Song::roundUpBar(int t) const
{
    int bar, beat;
    unsigned tick;
    sigmap.tickValues(t, &bar, &beat, &tick);
    if (beat || tick)
        return sigmap.bar2tick(bar + 1, 0, 0);
    return t;
}

//---------------------------------------------------------
//   roundUpBeat
//---------------------------------------------------------

int Song::roundUpBeat(int t) const
{
    int bar, beat;
    unsigned tick;
    sigmap.tickValues(t, &bar, &beat, &tick);
    if (tick)
        return sigmap.bar2tick(bar, beat + 1, 0);
    return t;
}

//---------------------------------------------------------
//   roundDownBar
//---------------------------------------------------------

int Song::roundDownBar(int t) const
{
    int bar, beat;
    unsigned tick;
    sigmap.tickValues(t, &bar, &beat, &tick);
    return sigmap.bar2tick(bar, 0, 0);
}

//---------------------------------------------------------
//   dumpMaster
//---------------------------------------------------------

void Song::dumpMaster()
{
    tempomap.dump();
    sigmap.dump();
}

//---------------------------------------------------------
//   getSelectedParts
//---------------------------------------------------------

PartList* Song::getSelectedMidiParts() const
{
    PartList* parts = new PartList();

    //------------------------------------------------------
    //    wenn ein Part selektiert ist, diesen editieren
    //    wenn ein Track selektiert ist, den Ersten
    //       Part des Tracks editieren, die restlichen sind
    //       'ghostparts'
    //    wenn mehrere Parts selektiert sind, dann Ersten
    //       editieren, die restlichen sind 'ghostparts'
    //
    // Rough translation:
    /*
          If a part is selected, edit that.
          If a track is selected, edit the first
           part of the track, the rest are
           'ghost parts'
          When multiple parts are selected, then edit the first,
            the rest are 'ghost parts'
     */


    // collect marked parts
    for (ciMidiTrack t = _midis.begin(); t != _midis.end(); ++t)
    {
        MidiTrack* track = *t;
        PartList* pl = track->parts();
        for (iPart p = pl->begin(); p != pl->end(); ++p)
        {
            if (p->second->selected())
            {
                parts->add(p->second);
            }
        }
    }
    // if no part is selected, then search for selected track
    // and collect all parts of this track

    if (parts->empty())
    {
        for (ciMidiTrack t = _tracks.begin(); t != _tracks.end(); ++t)
        {
            if ((*t)->selected())
            {
                MidiTrack* track = dynamic_cast<MidiTrack*> (*t);
                if (track == 0)
                    continue;
                PartList* pl = track->parts();
                for (iPart p = pl->begin(); p != pl->end(); ++p)
                    parts->add(p->second);
                break;
            }
        }
    }
    return parts;
}

void Song::setMidiType(MidiType t)
{
    //   printf("set MType %d\n", t);
    _mtype = t;
    song->update(SC_SONG_TYPE); // p4.0.7 Tim.
}

//---------------------------------------------------------
//   beat
//---------------------------------------------------------

void Song::beat()
{
    int tick = audio->tickPos();
    if (audio->isPlaying())
        setPos(0, tick, true, false, true);

    while (noteFifoSize)
    {
        int pv = recNoteFifo[noteFifoRindex];
        noteFifoRindex = (noteFifoRindex + 1) % REC_NOTE_FIFO_SIZE;
        int pitch = (pv >> 8) & 0xff;
        int velo = pv & 0xff;

        //---------------------------------------------------
        // filter midi remote control events
        //---------------------------------------------------

        if (rcEnable && velo != 0)
        {
            if (pitch == rcStopNote)
                setStop(true);
            else if (pitch == rcRecordNote)
                setRecord(true);
            else if (pitch == rcGotoLeftMarkNote)
                setPos(0, pos[LPOS].tick(), true, true, true);
            else if (pitch == rcPlayNote)
                setPlay(true);
        }
        emit song->midiNote(pitch, velo);
        --noteFifoSize;
    }
    if(!invalid)
    {
    }
}

//---------------------------------------------------------
//   setLen
//---------------------------------------------------------

void Song::setLen(unsigned l)
{
    _len = l;
    update();
}

//---------------------------------------------------------
//   addMarker
//---------------------------------------------------------

Marker* Song::addMarker(const QString& s, int t, bool lck)
{
    Marker* marker = _markerList->add(s, t, lck);
    emit markerChanged(MARKER_ADD);
    return marker;
}

//---------------------------------------------------------
//   addMarker
//---------------------------------------------------------

Marker* Song::getMarkerAt(int t)
{
    iMarker markerI;
    for (markerI = _markerList->begin(); markerI != _markerList->end(); ++markerI)
    {
        //                        if (i1->second.current())
        if (unsigned(t) == markerI->second.tick())//prevent of copmiler warning: comparison signed/unsigned
            return &markerI->second;
    }
    //Marker* marker = _markerList->add(s, t, lck);
    return NULL;
}

//---------------------------------------------------------
//   removeMarker
//---------------------------------------------------------

void Song::removeMarker(Marker* marker)
{
    _markerList->remove(marker);
    emit markerChanged(MARKER_REMOVE);
}

Marker* Song::setMarkerName(Marker* m, const QString& s)
{
    m->setName(s);
    emit markerChanged(MARKER_NAME);
    return m;
}

Marker* Song::setMarkerTick(Marker* m, int t)
{
    Marker mm(*m);
    _markerList->remove(m);
    mm.setTick(t);
    m = _markerList->add(mm);
    emit markerChanged(MARKER_TICK);
    return m;
}

Marker* Song::setMarkerLock(Marker* m, bool f)
{
    m->setType(f ? Pos::FRAMES : Pos::TICKS);
    emit markerChanged(MARKER_LOCK);
    return m;
}

//---------------------------------------------------------
//   setRecordFlag
//---------------------------------------------------------

void Song::setRecordFlag(MidiTrack* track, bool val, bool monitor)
{
    if(!monitor)
    {
        //Call the monitor here if it was not called from the monitor
        //midimonitor->msgSendMidiOutputEvent(track, CTRL_RECORD, val ? 127 : 0);
    }
    {
        track->setRecordFlag1(val, monitor);
        track->setRecordFlag2(val, monitor);
    }
    //      updateFlags |= SC_RECFLAG;
    update(SC_RECFLAG);

}

//---------------------------------------------------------
//   rescanAlsaPorts
//---------------------------------------------------------

void Song::rescanAlsaPorts()
{
    emit midiPortsChanged();
}

//---------------------------------------------------------
//   endMsgCmd
//---------------------------------------------------------

void Song::endMsgCmd()
{
    if (updateFlags)
    {
        redoList->clear(); // TODO: delete elements in list
        undoAction->setEnabled(true);
        redoAction->setEnabled(false);
        if(updateFlags && (SC_TRACK_REMOVED | SC_TRACK_INSERTED/* | SC_TRACK_MODIFIED*/))
        {
            //NOTE: This call was causing our mixer to update repeatedly, removed for now
            //keep an eye out for trackviews not updating after a undo/redo operation
            //printf("Song::endMsgCmd() calling updateTrackViews()\n");
            //updateTrackViews();
        }
        if(!invalid)
            emit songChanged(updateFlags);
    }
}

//---------------------------------------------------------
//   undo
//---------------------------------------------------------

void Song::undo()
{
    updateFlags = 0;
    if (doUndo1())
        return;
    audio->msgUndo();
    doUndo3();
    redoAction->setEnabled(true);
    undoAction->setEnabled(!undoList->empty());

    if (updateFlags && (SC_TRACK_REMOVED | SC_TRACK_INSERTED))
        audio->msgUpdateSoloStates();

    if(updateFlags && (SC_TRACK_REMOVED | SC_TRACK_INSERTED | SC_TRACK_MODIFIED))
        updateTrackViews();

    if(!invalid)
        emit songChanged(updateFlags);
}

//---------------------------------------------------------
//   redo
//---------------------------------------------------------

void Song::redo()
{
    updateFlags = 0;
    if (doRedo1())
        return;
    audio->msgRedo();
    doRedo3();
    undoAction->setEnabled(true);
    redoAction->setEnabled(!redoList->empty());

    if (updateFlags && (SC_TRACK_REMOVED | SC_TRACK_INSERTED))
        audio->msgUpdateSoloStates();

    if(updateFlags && (SC_TRACK_REMOVED | SC_TRACK_INSERTED | SC_TRACK_MODIFIED))
        updateTrackViews();
    if(!invalid)
        emit songChanged(updateFlags);
}

//---------------------------------------------------------
//   processMsg
//    executed in realtime thread context
//---------------------------------------------------------

void Song::processMsg(AudioMsg* msg)
{
    switch (msg->id)
    {
        case SEQM_UPDATE_SOLO_STATES:
            updateSoloStates();
            break;
        case SEQM_UNDO:
            doUndo2();
            break;
        case SEQM_REDO:
            doRedo2();
            break;
        case SEQM_MOVE_TRACK:
            if (msg->a > msg->b)
            {
                for (int i = msg->a; i > msg->b; --i)
                {
                    swapTracks(i, i - 1);
                }
            }
            else
            {
                for (int i = msg->a; i < msg->b; ++i)
                {
                    swapTracks(i, i + 1);
                }
            }
            updateFlags = SC_TRACK_MODIFIED;
            break;
        case SEQM_ADD_EVENT:
            updateFlags = SC_EVENT_INSERTED;
            if (addEvent(msg->ev1, (MidiPart*) msg->p2))
            {
                Event ev;
                //undoOp(UndoOp::AddEvent, ev, msg->ev1, (Part*)msg->p2);
                undoOp(UndoOp::AddEvent, ev, msg->ev1, (Part*) msg->p2, msg->a, msg->b);
            }
            else
                updateFlags = 0;
            if (msg->a)
                addPortCtrlEvents(msg->ev1, (Part*) msg->p2, msg->b);
            break;
        case SEQM_ADD_EVENT_CHECK:
        {
            Track* track = msg->track;
            Event event = msg->ev1;
            unsigned tick = event.tick();
            PartList* pl = track->parts();
            if(pl && !pl->empty())
            {
                Part* part = pl->findAtTick(tick);
                if(part)
                {
                    //recordEvent((MidiPart*)part, event);
                    //unsigned tick = event.tick();
                    int diff = event.endTick() - part->lenTick();
                    if (diff > 0)
                    {// too short part? extend it
                        part->setLenTick(part->lenTick() + diff);
                        updateFlags |= SC_PART_MODIFIED;
                    }
                    updateFlags |= SC_EVENT_INSERTED;

                    tick -= part->tick();
                    event.setTick(tick);

                    Event ev;
                    if (event.type() == Controller)
                    {
                        EventRange range = part->events()->equal_range(tick);
                        for (iEvent i = range.first; i != range.second; ++i)
                        {
                            ev = i->second;
                            // At the moment, Song::recordEvent() is only called by the 'Rec' buttons in the
                            //  midi track info panel. So only controller types are fed to it. If other event types
                            //  are to be passed, we will have to expand on this to check if equal. Instead, maybe add an isEqual() to Event class.
                            if (ev.type() == Controller && ev.dataA() == event.dataA())
                            {
                                // Don't bother if already set.
                                if (ev.dataB() == event.dataB())
                                    return;
                                removePortCtrlEvents(ev, (MidiPart*) part, true);
                                changeEvent(ev, event, (MidiPart*) part);
                                addPortCtrlEvents(event, part, true);
                                undoOp(UndoOp::ModifyEvent, event, ev, part, true, true);
                                return;
                            }
                        }
                    }

                    if (addEvent(event, (MidiPart*) part))
                    {
                        Event ev;
                        undoOp(UndoOp::AddEvent, ev, event, part, true, true);
                    }
                    addPortCtrlEvents(event, part, true);
                }
            }
        }
        break;
        case SEQM_REMOVE_EVENT:
        {
            Event event = msg->ev1;
            MidiPart* part = (MidiPart*) msg->p2;
            if (msg->a)
                removePortCtrlEvents(event, part, msg->b);
            Event e;
            undoOp(UndoOp::DeleteEvent, e, event, (Part*) part, msg->a, msg->b);
            deleteEvent(event, part);
            updateFlags = SC_EVENT_REMOVED;
        }
            break;
        case SEQM_CHANGE_EVENT:
            if (msg->a)
                removePortCtrlEvents(msg->ev1, (MidiPart*) msg->p3, msg->b);
            changeEvent(msg->ev1, msg->ev2, (MidiPart*) msg->p3);
            if (msg->a)
                addPortCtrlEvents(msg->ev2, (Part*) msg->p3, msg->b);
            undoOp(UndoOp::ModifyEvent, msg->ev2, msg->ev1, (Part*) msg->p3, msg->a, msg->b);
            updateFlags = SC_EVENT_MODIFIED;
            break;

        case SEQM_ADD_TEMPO:
            //printf("processMsg (SEQM_ADD_TEMPO) UndoOp::AddTempo. adding tempo at: %d with tempo=%d\n", msg->a, msg->b);
            undoOp(UndoOp::AddTempo, msg->a, msg->b);
            tempomap.addTempo(msg->a, msg->b);
            updateFlags = SC_TEMPO;
            break;

        case SEQM_SET_TEMPO:
            //printf("processMsg (SEQM_SET_TEMPO) UndoOp::AddTempo. adding tempo at: %d with tempo=%d\n", msg->a, msg->b);
            undoOp(UndoOp::AddTempo, msg->a, msg->b);
            tempomap.setTempo(msg->a, msg->b);
            updateFlags = SC_TEMPO;
            break;

        case SEQM_SET_GLOBAL_TEMPO:
            tempomap.setGlobalTempo(msg->a);
            break;

        case SEQM_REMOVE_TEMPO:
            //printf("processMsg (SEQM_REMOVE_TEMPO) UndoOp::DeleteTempo. adding tempo at: %d with tempo=%d\n", msg->a, msg->b);
            undoOp(UndoOp::DeleteTempo, msg->a, msg->b);
            tempomap.delTempo(msg->a);
            updateFlags = SC_TEMPO;
            break;

        case SEQM_REMOVE_TEMPO_RANGE:
        {
            //printf("processMsg (SEQM_REMOVE_TEMPO) UndoOp::DeleteTempo. adding tempo at: %d with tempo=%d\n", msg->a, msg->b);
            QList<void*> list = msg->objectList;
            if(!list.isEmpty())
            {
                TEvent* se = (TEvent*)list.front();
                TEvent* ee = (TEvent*)list.back();
                for(int i = 0; i < list.size(); i++)
                {
                    TEvent* ev = (TEvent*)list.at(i);
                    undoOp(UndoOp::DeleteTempo, ev->tick, ev->tempo);
                }
                tempomap.delTempoRange(se->tick, ee->tick);
                updateFlags = SC_TEMPO;
            }
            break;
        }
        case SEQM_ADD_SIG:
            undoOp(UndoOp::AddSig, msg->a, msg->b, msg->c);
            sigmap.add(msg->a, msg->b, msg->c);
            updateFlags = SC_SIG;
            break;

        case SEQM_REMOVE_SIG:
            undoOp(UndoOp::DeleteSig, msg->a, msg->b, msg->c);
            sigmap.del(msg->a);
            updateFlags = SC_SIG;
            break;

        default:
            printf("unknown seq message %d\n", msg->id);
            break;
    }
}

//---------------------------------------------------------
//   cmdAddPart
//---------------------------------------------------------

void Song::cmdAddPart(Part* part)
{
    addPart(part);
    undoOp(UndoOp::AddPart, part);
    updateFlags = SC_PART_INSERTED;
}

//---------------------------------------------------------
//   cmdRemovePart
//---------------------------------------------------------

void Song::cmdRemovePart(Part* part)
{
    removePart(part);
    undoOp(UndoOp::DeletePart, part);
    part->events()->incARef(-1);
    unchainClone(part);
    updateFlags = SC_PART_REMOVED;
}

//---------------------------------------------------------
//   cmdChangePart
//---------------------------------------------------------

void Song::cmdChangePart(Part* oldPart, Part* newPart, bool doCtrls, bool doClones)
{
    //printf("Song::cmdChangePart before changePart oldPart:%p events:%p refs:%d Arefs:%d sn:%d newPart:%p events:%p refs:%d Arefs:%d sn:%d\n", oldPart, oldPart->events(), oldPart->events()->refCount(), oldPart->events()->arefCount(), oldPart->sn(), newPart, newPart->events(), newPart->events()->refCount(), newPart->events()->arefCount(), newPart->sn());

    if (doCtrls)
        removePortCtrlEvents(oldPart, doClones);

    changePart(oldPart, newPart);

    undoOp(UndoOp::ModifyPart, oldPart, newPart, doCtrls, doClones);

    // Changed by T356. Do not decrement ref count if the new part is a clone of the old part, since the event list
    //  will still be active.
    if (oldPart->cevents() != newPart->cevents())
        oldPart->events()->incARef(-1);

    //printf("Song::cmdChangePart before repl/unchClone oldPart:%p events:%p refs:%d Arefs:%d sn:%d newPart:%p events:%p refs:%d Arefs:%d sn:%d\n", oldPart, oldPart->events(), oldPart->events()->refCount(), oldPart->events()->arefCount(), oldPart->sn(), newPart, newPart->events(), newPart->events()->refCount(), newPart->events()->arefCount(), newPart->sn());

    replaceClone(oldPart, newPart);

    if (doCtrls)
        addPortCtrlEvents(newPart, doClones);

    //printf("Song::cmdChangePart after repl/unchClone oldPart:%p events:%p refs:%d Arefs:%d sn:%d newPart:%p events:%p refs:%d Arefs:%d sn:%d\n", oldPart, oldPart->events(), oldPart->events()->refCount(), oldPart->events()->arefCount(), oldPart->sn(), newPart, newPart->events(), newPart->events()->refCount(), newPart->events()->arefCount(), newPart->sn());

    updateFlags = SC_PART_MODIFIED;
    //update(updateFlags);
}

//---------------------------------------------------------
//   panic
//---------------------------------------------------------

void Song::panic()
{
    audio->msgPanic();
}

//---------------------------------------------------------
//   clear
//    signal - emit signals for changes if true
//    called from constructor as clear(false) and
//    from LOS::clearSong() as clear(false)
//---------------------------------------------------------

void Song::clear(bool signal)
{
    if (debugMsg)
        printf("Song::clear\n");

    m_tracks.clear();
    m_trackIndex.clear();
    m_arrangerTracks.clear();
    m_trackViewIndex.clear();
    m_arrangerTrackIndex.clear();
    _tviews.clear();
    _tracks.clear();
    _artracks.clear();
    _viewtracks.clear();
    _midis.clearDelete();

    //Clear all midi port devices.
    for (int i = 0; i < kMaxMidiPorts; ++i)
    {
        //Since midi ports are not deleted, clear all midi port in/out routes. They point to non-existant tracks now.
        midiPorts[i].inRoutes()->clear();
        midiPorts[i].outRoutes()->clear();

        //Clear out the patch sequences between song load
        //This causes a crash right now only when the PR is open while changing songs
        //Its because all the components of PR like trackinfo are not reacting properly
        //I think the solution is to close the PR before changing songs, what's the point
        //of having it there between completely different songs.
        midiPorts[i].patchSequences()->clear();

        //Reset this.
        midiPorts[i].setFoundInSongFile(false);

        // This will also close the device.
        midiPorts[i].setMidiDevice(0);
    }

    // Make sure to delete Jack midi devices, and remove all ALSA midi device routes...
    QList<MidiDevice*> deleteList;
    for (iMidiDevice imd = midiDevices.begin(); imd != midiDevices.end(); ++imd)
    {
        MidiDevice* md = (MidiDevice*)*imd;
        if(md)
        {
            int type = md->deviceType();
            switch(type)
            {
                case MidiDevice::JACK_MIDI:
                {
                    deleteList.append(md);
                }
                break;
                case MidiDevice::ALSA_MIDI:
                {
                    // With alsa devices, we must not delete them (they're always in the list). But we must
                    //  clear all routes. They point to non-existant midi tracks, which were all deleted above.
                    (*imd)->inRoutes()->clear();
                    (*imd)->outRoutes()->clear();
                }
                break;
            }
        }
    }

    //Now safely delete them from the device list
    foreach(MidiDevice* md, deleteList)
    {
        int type = md->deviceType();
        switch(type)
        {
            case MidiDevice::JACK_MIDI:
            {
                // Remove the device from the list.
                midiDevices.remove(md);
                // Since Jack midi devices are created dynamically, we must delete them.
                // The destructor unregisters the device from Jack, which also disconnects all device-to-jack routes.
                // This will also delete all midi-track-to-device routes, they point to non-existant midi tracks
                //  which were all deleted above
                delete md;
            }
            break;
            default://ALSA already handled
            break;
        }
    }

    tempomap.clear();
    sigmap.clear();
    undoList->clearDelete();
    redoList->clear();
    m_undoStack->clear();
    _markerList->clear();
    pos[0].setTick(0);
    pos[1].setTick(0);
    pos[2].setTick(0);
    _vcpos.setTick(0);

    Track::clearSoloRefCounts();
    clearMidiTransforms();
    clearMidiInputTransforms();

    // Clear all midi port controller values.
    for (int i = 0; i < kMaxMidiPorts; ++i)
        // Don't remove the controllers, just the values.
        midiPorts[i].controller()->clearDelete(false);

    _masterFlag = true;
    loopFlag = false;
    loopFlag = false;
    punchinFlag = false;
    punchoutFlag = false;
    recordFlag = false;
    soloFlag = false;
    // seq
    _mtype = MIDI_TYPE_NULL;
    _recMode = REC_OVERDUP;
    _cycleMode = CYCLE_NORMAL;
    _quantize = false;
    _len = 405504; // song len in ticks
    _follow = JUMP;
    // _tempo      = 500000;      // default tempo 120
    dirty = false;

    if (signal)
    {
        emit loopChanged(false);
        recordChanged(false);
    }
}

//---------------------------------------------------------
//   cleanupForQuit
//   called from LOS::closeEvent
//---------------------------------------------------------

void Song::cleanupForQuit()
{
    invalid = true;

    if (debugMsg)
        printf("LOS: Song::cleanupForQuit...\n");

    m_tracks.clear();
    m_trackIndex.clear();
    m_arrangerTracks.clear();
    m_trackViewIndex.clear();
    m_arrangerTrackIndex.clear();
    _tracks.clear();
    _artracks.clear();
    _viewtracks.clear();

    if (debugMsg)
        printf("deleting _midis\n");
    _midis.clearDelete();

    tempomap.clear();
    sigmap.clear();

    if (debugMsg)
        printf("deleting undoList, clearing redoList\n");
    undoList->clearDelete();
    redoList->clear(); // Check this - Should we do a clearDelete? IIRC it was OK this way - no clearDelete in case of same items in both lists.

    _markerList->clear();

    _tviews.clear();

    if (debugMsg)
        printf("deleting transforms\n");
    clearMidiTransforms(); // Deletes stuff.
    clearMidiInputTransforms(); // Deletes stuff.

    if (debugMsg)
        printf("deleting midiport controllers\n");
    // Clear all midi port controllers and values.
    for (int i = 0; i < kMaxMidiPorts; ++i)
    {
        //Clear out the patch sequences
        midiPorts[i].patchSequences()->clear();

        // Remove the controllers and the values.
        midiPorts[i].controller()->clearDelete(true);
    }

    // Can't do this here. Jack isn't running. Fixed. Test OK so far.
#if 1
    if (debugMsg)
        printf("deleting midi devices except synths\n");
    for (iMidiDevice imd = midiDevices.begin(); imd != midiDevices.end(); ++imd)
    {
        delete (*imd);
    }
    midiDevices.clear(); // midi devices
#endif

    if (debugMsg)
        printf("deleting midi instruments\n");
    for (iMidiInstrument imi = midiInstruments.begin(); imi != midiInstruments.end(); ++imi)
        delete (*imi);
    midiInstruments.clear(); // midi devices

    // Nothing required for ladspa plugin list, and rack instances of them
    //  are handled by ~AudioTrack.

    invalid = true;
    if (debugMsg)
        printf("...finished cleaning up.\n");
}

void Song::playMonitorEvent(int fd)
{
    int size = sizeof(MonitorData);
    //char buffer[16];
    char buffer[size];

    //int n = ::read(fd, buffer, 16);
    int n = ::read(fd, buffer, size);
    if (n < 0)
    {
        printf("Song: playMonitorEvent(): READ PIPE failed: %s\n",
                strerror(errno));
        return;
    }
    processMonitorMessage(buffer);
}
void Song::processMonitorMessage(const void* m)
{
    MonitorData* mdata = (MonitorData*)m;
    if(mdata)
    {
        //FIXME: For NRPN Support this needs to take into account the Controller type
        switch(mdata->dataType)
        {
            case MIDI_INPUT:
            {
                MidiPlayEvent ev(0, mdata->port, mdata->channel, ME_CONTROLLER, mdata->controller, mdata->value, mdata->track);
                ev.setEventSource(MonitorSource);
                midiPorts[ev.port()].sendEvent(ev);
                return;
            }
            break;
            case MIDI_LEARN:
            {
                //Values are: Port, Channel, CC
                emit midiLearned(mdata->port, mdata->channel, mdata->controller);
            }
            break;
            case MIDI_LEARN_NRPN:
            {//The fifth param is just a padding
                //Values are: Port, Channel, MSB, LSB
                emit midiLearned(mdata->port, mdata->channel, mdata->msb, mdata->lsb);
            }
            break;
        }
    }
}

//---------------------------------------------------------
//   seqSignal
//    sequencer message to GUI
//    execution environment: gui thread
//---------------------------------------------------------

void Song::seqSignal(int fd)/*{{{*/
{
    char buffer[16];

    int n = ::read(fd, buffer, 16);
    if (n < 0)
    {
        printf("Song: seqSignal(): READ PIPE failed: %s\n",
                strerror(errno));
        return;
    }
    for (int i = 0; i < n; ++i)
    {
        // printf("seqSignal to gui:<%c>\n", buffer[i]);
        switch (buffer[i])
        {
            case '0': // STOP
                stopRolling();
                break;
            case '1': // PLAY
                setStopPlay(true);
                break;
            case '2': // record
                setRecord(true);
                break;
            case '3': // START_PLAY + jack STOP
                abortRolling();
                break;
            case 'P': // alsa ports changed
                rescanAlsaPorts();
                break;
            case 'G':
                setPos(0, audio->tickPos(), true, false, true);
                break;
            case 'S': // shutdown audio
                los->seqStop();

            {
                // give the user a sensible explanation
                jackErrorBox = new QMessageBox(QMessageBox::Critical, tr("Jack shutdown!"),
                //int btn = QMessageBox::critical(los, tr("Jack shutdown!"),
                        tr("Jack has detected a performance problem which has lead to\n"
                        "LOS being disconnected.\n"
                        "This could happen due to a number of reasons:\n"
                        "- a performance issue with your particular setup.\n"
                        "- a bug in LOS (or possibly in another connected software).\n"
                        "- a random hiccup which might never occur again.\n"
                        "- jack was voluntary stopped by you or someone else\n"
                        "- jack crashed\n"
                        "If there is a persisting problem you are much welcome to discuss it\n"
                        "on the LOS mailinglist.\n"
                        "(there is information about joining the mailinglist on the LOS\n"
                        " homepage which is available through the help menu)\n"
                        "\n"
                        "To proceed check the status of Jack and try to restart it and then .\n"
                        "click \"Audio > Restart Audio\" menu."), QMessageBox::Close, los);
                jackErrorBox->exec();
                /*if (btn == 0)
                {
                    printf("restarting!\n");
                    los->seqRestart();
                }*/
            }

                break;

            case 'C': // Graph changed
                if (audioDevice)
                    audioDevice->graphChanged();
                break;

            case 'R': // Registration changed
                if (audioDevice)
                    audioDevice->registrationChanged();
                break;

            default:
                printf("unknown Seq Signal <%c>\n", buffer[i]);
                break;
        }
    }
}/*}}}*/

void Song::closeJackBox()
{
    if(jackErrorBox)
    {
        jackErrorBox->close();
        jackErrorBox = 0;
    }
}

//---------------------------------------------------------
//   recordEvent
//---------------------------------------------------------

void Song::recordEvent(MidiTrack* mt, Event& event)/*{{{*/
{
    //---------------------------------------------------
    //    if tick points into a part,
    //          record to that part
    //    else
    //          create new part
    //---------------------------------------------------

    unsigned tick = event.tick();
    PartList* pl = mt->parts();
    MidiPart* part = 0;
    iPart ip;
    for (ip = pl->begin(); ip != pl->end(); ++ip)
    {
        part = (MidiPart*) (ip->second);
        unsigned partStart = part->tick();
        unsigned partEnd = partStart + part->lenTick();
        if (tick >= partStart && tick < partEnd)
            break;
    }
    updateFlags |= SC_EVENT_INSERTED;
    if (ip == pl->end())
    {
        // create new part
        part = new MidiPart(mt);
        int startTick = roundDownBar(tick);
        //int endTick   = roundUpBar(tick);
        int endTick = roundUpBar(tick + 1);
        part->setTick(startTick);
        part->setLenTick(endTick - startTick);
        part->setName(mt->name());
        event.move(-startTick);
        part->events()->add(event);
        audio->msgAddPart(part);
        return;
    }
    part = (MidiPart*) (ip->second);
    tick -= part->tick();
    event.setTick(tick);

    Event ev;
    if (event.type() == Controller)
    {
        EventRange range = part->events()->equal_range(tick);
        for (iEvent i = range.first; i != range.second; ++i)
        {
            ev = i->second;
            // At the moment, Song::recordEvent() is only called by the 'Rec' buttons in the
            //  midi track info panel. So only controller types are fed to it. If other event types
            //  are to be passed, we will have to expand on this to check if equal. Instead, maybe add an isEqual() to Event class.
            //if((ev.type() == Controller && event.type() == Controller || ev.type() == Controller && event.type() == Controller)
            //   && ev.dataA() == event.dataA() && ev.dataB() == event.dataB())
            if (ev.type() == Controller && ev.dataA() == event.dataA())
            {
                // Don't bother if already set.
                if (ev.dataB() == event.dataB())
                    return;
                // Indicate do undo, and do port controller values and clone parts.
                audio->msgChangeEvent(ev, event, part, true, true, true);
                return;
            }
        }
    }

    // Indicate do undo, and do port controller values and clone parts.
    //audio->msgAddEvent(event, part);
    audio->msgAddEvent(event, part, true, true, true);
}/*}}}*/

void Song::recordEvent(MidiPart* part, Event& event)/*{{{*/
{
    //---------------------------------------------------
    //    if tick points into a part,
    //          record to that part
    //    else
    //          create new part
    //---------------------------------------------------

    unsigned tick = event.tick();
    int diff = event.endTick() - part->lenTick();
    if (diff > 0)
    {// too short part? extend it
        Part* newPart = part->clone();
        newPart->setLenTick(newPart->lenTick() + diff);
        // Indicate no undo, and do port controller values but not clone parts.
        audio->msgChangePart(part, newPart, false, true, false);
        updateFlags |= SC_PART_MODIFIED;
        part = (MidiPart*)newPart;
    }
    updateFlags |= SC_EVENT_INSERTED;

    tick -= part->tick();
    event.setTick(tick);

    Event ev;
    if (event.type() == Controller)
    {
        EventRange range = part->events()->equal_range(tick);
        for (iEvent i = range.first; i != range.second; ++i)
        {
            ev = i->second;
            // At the moment, Song::recordEvent() is only called by the 'Rec' buttons in the
            //  midi track info panel. So only controller types are fed to it. If other event types
            //  are to be passed, we will have to expand on this to check if equal. Instead, maybe add an isEqual() to Event class.
            if (ev.type() == Controller && ev.dataA() == event.dataA())
            {
                // Don't bother if already set.
                if (ev.dataB() == event.dataB())
                    return;
                // Indicate do undo, and do port controller values and clone parts.
                audio->msgChangeEvent(ev, event, part, true, true, true);
                return;
            }
        }
    }

    // Indicate do undo, and do port controller values and clone parts.
    audio->msgAddEvent(event, part, true, true, true);
}/*}}}*/

//---------------------------------------------------------
//   updateSoloStates
//    This will properly set all soloing variables (including other tracks) based entirely
//     on the current values of all the tracks' _solo members.
//---------------------------------------------------------

void Song::updateSoloStates()
{
    Track::clearSoloRefCounts();
    for (ciMidiTrack i = _tracks.begin(); i != _tracks.end(); ++i)
        (*i)->setInternalSolo(0);
    for (ciMidiTrack i = _tracks.begin(); i != _tracks.end(); ++i)
        (*i)->updateSoloStates(true);
}

//---------------------------------------------------------
//   abortRolling
//---------------------------------------------------------

void Song::abortRolling()
{
    if (record())
        audio->recordStop();
    setStopPlay(false);
}

//---------------------------------------------------------
//   stopRolling
//---------------------------------------------------------

void Song::stopRolling()
{
    abortRolling();
}

//---------------------------------------------------------
// insertTrackView
//    add a new trackview for the Arranger
//---------------------------------------------------------

void Song::insertTrackView(TrackView* tv, int idx)
{
    _tviews[tv->id()] = tv;
    m_trackViewIndex.insert(idx, tv->id());
    //TODO: This should not be called update(SC_VIEW_ADDED) instead
    updateTrackViews();
}

//---------------------------------------------------------
//   cmdRemoveTrackView
//---------------------------------------------------------

void Song::cmdRemoveTrackView(qint64 id)
{
    removeTrackView(id);
    updateFlags |= SC_TRACKVIEW_REMOVED;
}

//---------------------------------------------------------
// removeTrackView
//    add a new trackview for the Arranger
//---------------------------------------------------------

void Song::removeTrackView(qint64 id)
{
    _tviews.erase(_tviews.find(id));
    m_trackViewIndex.removeAll(id);
    update(SC_TRACKVIEW_REMOVED);
    updateTrackViews();
}

//---------------------------------------------------------
// addNewTrackView
//    add a new trackview for the Arranger
//---------------------------------------------------------

TrackView* Song::addNewTrackView()
{
    TrackView* tv = addTrackView();
    return tv;
}

//---------------------------------------------------------
//    addTrackView
//    called from GUI context
//---------------------------------------------------------

TrackView* Song::addTrackView()/*{{{*/
{
    TrackView* tv = new TrackView();
    tv->setDefaultName();
    _tviews[tv->id()] = tv;
    m_trackViewIndex.append(tv->id());
    //msgInsertTrackView(tv, -1, true);

    return tv;
}/*}}}*/

//---------------------------------------------------------
//   findTrackView
//    find track view by name
//---------------------------------------------------------

TrackView* Song::findTrackViewById(qint64 id) const
{
    if(_tviews.contains(id))
        return _tviews.value(id);
    return 0;
}

//---------------------------------------------------------
//   findTrackViewById
//    find internal trackview by id
//---------------------------------------------------------

TrackView* Song::findAutoTrackViewById(qint64 id) const
{
    if(_autotviews.contains(id))
        return _autotviews.value(id);
    return 0;
}

//---------------------------------------------------------
//   findAutoTrackView
//    find track view by name
//---------------------------------------------------------

TrackView* Song::findAutoTrackView(const QString& name) const
{
    QHashIterator<qint64, TrackView*> iter(_autotviews);
    while(iter.hasNext())
    {
        iter.next();
        if (iter.value()->viewName() == name)
            return iter.value();
    }
    return 0;
}


//---------------------------------------------------------
//   findTrackViewByTrackId
//    find track view by track ID
//---------------------------------------------------------

TrackView* Song::findTrackViewByTrackId(qint64 tid)
{
    QHashIterator<qint64, TrackView*> iter(_tviews);
    while(iter.hasNext())
    {
        iter.next();
        QList<qint64> *list = iter.value()->trackIndexList();
        if(list->contains(tid))
            return iter.value();
    }
    return 0;
}

//---------------------------------------------------------
// updateTrackViews
//---------------------------------------------------------
void Song::updateTrackViews()
{
    //printf("Song::updateTrackViews()\n");
    _viewtracks.clear();
    m_trackIndex.clear();
    m_arrangerTracks.clear();
    m_viewTracks.clear();
    //Create omnipresent Master track at top of all list.
    MidiTrack* master = findTrack("Master");
    if(master)
    {
        _viewtracks.push_back(master);
        m_arrangerTracks[master->id()] = master;
        m_viewTracks[master->id()] = master;
        m_trackIndex.insert(0, master->id());
    }
    viewselected = false;
    bool customview = false;
    bool workview = false;
    bool commentview = false;
    TrackView* wv = _autotviews.value(m_workingViewId);
    TrackView* cv = _autotviews.value(m_commentViewId);
    if(wv && wv->selected())
    {
        workview = true;
        viewselected = true;
    }
    if(cv && cv->selected())
    {
        commentview = true;
        viewselected = true;
    }
    foreach(qint64 tvid, m_trackViewIndex)
    {
        TrackView* it = _tviews.value(tvid);
        if(it && it->selected())
        {
            //qDebug("Song::updateTrackViews Found TrackView %s", it->viewName().toUtf8().constData());
            customview = true;
            viewselected = true;
            QList<qint64> *tlist = it->trackIndexList();
            QMap<qint64, TrackView::TrackViewTrack*> *tvlist = it->tracks();
            for(int c = 0; c < tlist->size(); ++c)
            {
                qint64 tid = tlist->at(c);

                TrackView::TrackViewTrack *tvt = tvlist->value(tid);
                if(tvt)
                {
                    //qDebug("Song::updateTrackViews found TrackView::TrackViewTrack for view ~~~~~~~~~~~%lld", tid);
                    if(tvt->is_virtual)
                    {//Do nothing here, this is handled in TrackViewDock::updateTrackView
                        //qDebug("Song::updateTrackViews found virtual track skipping here ~~~~~~~~~~~%lld", tvt->id);
                        continue;
                    }
                    else
                    {
                        MidiTrack *t = findTrackById(tid);
                        if(t)
                        {
                            //qDebug("Song::updateTrackViews found track in song ~~~~~~~~~~~ %s", t->name().toUtf8().constData());
                            bool found = false;
                            t->setSelected(false);
                            if(workview && t->parts()->empty()) {
                                continue;
                            }
                            //printf("Adding track to view %s\n", (*t)->name().toStdString().c_str());
                            if(m_viewTracks.contains(tid))
                            {
                                found = true;
                                //Make sure to record arm the ones that were in other views as well
                                t->setRecordFlag1(it->record());
                                t->setRecordFlag2(it->record());
                            }
                            if(!found)
                            {
                                //qDebug("Song::updateTrackViews adding track view ~~~~~~~~~~~ %s", t->name().toUtf8().constData());
                                _viewtracks.push_back(t);
                                m_viewTracks[tid] = t;
                                t->setRecordFlag1(it->record());
                                t->setRecordFlag2(it->record());
                                m_trackIndex.append(it->id());
                            }
                        }
                    }
                }
            }
        }
    }

    foreach(qint64 tvid, m_autoTrackViewIndex)
    {
        TrackView* ait = _autotviews.value(tvid);
        if(customview && (ait && ait->id() == m_workingViewId))
        {
            //qDebug("Song::updateTrackViews: skipping working View");
            continue;
        }
        if(ait && ait->selected())/*{{{*/
        {
            if(debugMsg)
                qDebug("Song::updateTrackViews: Selected View: %s", ait->viewName().toUtf8().constData());
            viewselected = true;
            QList<qint64> *tlist = ait->trackIndexList();
            QMap<qint64, TrackView::TrackViewTrack*> *tvlist = ait->tracks();
            for(int c = 0; c < tlist->size(); ++c)
            {
                qint64 i = tlist->at(c);
                TrackView::TrackViewTrack *tvt = tvlist->value(i);
                if(tvt)
                {//There will never be virtual tracks in these views
                    MidiTrack* t = m_tracks.value(tvt->id);
                    if(t)
                    {
                        t->setSelected(false);
                        if(t == master)
                            continue;
                        if(!m_viewTracks.contains(t->id()))
                        {
                            if(workview && t->parts()->empty())
                                continue;
                            if(t->comment().isEmpty() && commentview)
                                continue;
                            _viewtracks.push_back(t);
                            m_viewTracks[t->id()] = t;
                            m_trackIndex.append(t->id());
                        }
                    }
                }
            }
        }/*}}}*/
    }

    if(!viewselected)
    {
        //Make the viewtracks the artracks
        for(ciMidiTrack it = _artracks.begin(); it != _artracks.end(); ++it)
        {
            _viewtracks.push_back((*it));
            m_viewTracks[(*it)->id()] = (*it);
            m_trackIndex.append((*it)->id());
        }
    }
    //printf("Song::updateTrackViews() took %f seconds to run\n", end-start);
    disarmAllTracks();
    if(!invalid)
        emit arrangerViewChanged();
}

//---------------------------------------------------------
//   insertTrack
//---------------------------------------------------------

void Song::insertTrack(MidiTrack* track, int idx)
{
    insertTrack1(track, idx);
    insertTrackRealtime(track, idx); // audio->msgInsertTrack(track, idx, false);
}

//---------------------------------------------------------
//   insertTrack1
//    non realtime part of insertTrack
//---------------------------------------------------------

void Song::insertTrack1(MidiTrack* track, int /*idx*/)
{
    //printf("Song::insertTrack1 track:%lx\n", track);

    //printf("Song::insertTrack1 end of function\n");

}

//---------------------------------------------------------
//   insertTrackRealtime
//    realtime part
//---------------------------------------------------------

void Song::insertTrackRealtime(MidiTrack* track, int idx)
{
    //printf("Song::insertTrackRealtime track:%lx\n", track);

    iMidiTrack ia;
    {
            _midis.push_back((MidiTrack*) track);
            ia = _artracks.index2iterator(idx);
            _artracks.insert(ia, track);
            addPortCtrlEvents(((MidiTrack*) track));
            m_arrangerTracks[track->id()] = track;
            m_arrangerTrackIndex.insert(idx, track->id());
            _autotviews.value(m_workingViewId)->addTrack(track->id());
    }

    //
    // initialize missing aux send
    //
    iMidiTrack i = _tracks.index2iterator(idx);
    //printf("Song::insertTrackRealtime inserting into _tracks...\n");

    _tracks.insert(i, track);
    m_tracks[track->id()] = track;
    m_trackIndex.insert(idx, track->id());
    _autotviews.value(m_commentViewId)->addTrack(track->id());
    //printf("Song::insertTrackRealtime inserted\n");

    //
    //  add routes
    //

    {
        const RouteList* rl = track->inRoutes();
        for (ciRoute r = rl->begin(); r != rl->end(); ++r)
        {
            //printf("Song::insertTrackRealtime %s in route port:%d\n", track->name().toLatin1().constData(), r->midiPort);
            Route src(track, r->channel);
            midiPorts[r->midiPort].outRoutes()->push_back(src);
        }
        rl = track->outRoutes();
        for (ciRoute r = rl->begin(); r != rl->end(); ++r)
        {
            //printf("Song::insertTrackRealtime %s out route port:%d\n", track->name().toLatin1().constData(), r->midiPort);
            Route src(track, r->channel);
            midiPorts[r->midiPort].inRoutes()->push_back(src);
        }
    }

    //printf("Song::insertTrackRealtime end of function\n");

}

//---------------------------------------------------------
//   removeTrack
//---------------------------------------------------------

void Song::removeTrack(MidiTrack* track)
{
    removeTrack1(track);
    audio->msgRemoveTrack(track);
    //delete track;
    update(SC_TRACK_REMOVED);
}

//---------------------------------------------------------
//   removeTrack1
//    non realtime part of removeTrack
//---------------------------------------------------------

void Song::removeTrack1(MidiTrack*)
{
}

//---------------------------------------------------------
//   removeTrackRealtime
//    called from RT context
//---------------------------------------------------------

void Song::removeTrackRealtime(MidiTrack* track)
{
    //printf("Song::removeTrackRealtime track:%s\n", track->name().toLatin1().constData());
    midiMonitor->msgDeleteMonitoredTrack(track);

    {
            removePortCtrlEvents(((MidiTrack*) track));
            unchainTrackParts(track, true);

            _midis.erase(track);
            _artracks.erase(track);
            m_arrangerTracks.erase(m_arrangerTracks.find(track->id()));
            _autotviews.value(m_workingViewId)->removeTrack(track->id());
    }
    _tracks.erase(track);
    m_tracks.erase(m_tracks.find(track->id()));
    m_trackIndex.removeAll(track->id());
    _autotviews.value(m_commentViewId)->removeTrack(track->id());
    TrackView* tv = findTrackViewByTrackId(track->id());
    bool updateview = false;
    while(tv)
    {
        updateview = true;
        tv->removeTrack(track->id());
        tv = findTrackViewByTrackId(track->id());
    }
    updateTrackViews();


    //
    //  remove routes
    //

    {
        //qDebug("Song::removeTrackRealtime: ~~~~~~~~~~~~~~~~~~~~Removing input routes for midi track");
        const RouteList* rl = track->inRoutes();
        for (ciRoute r = rl->begin(); r != rl->end(); ++r)
        {
            //printf("Song::removeTrackRealtime %s in route port:%d\n", track->name().toLatin1().constData(), r->midiPort);
            Route src(track, r->channel);
            midiPorts[r->midiPort].outRoutes()->removeRoute(src);
        }
        //qDebug("Song::removeTrackRealtime: ~~~~~~~~~~~~~~~~~~~~Removing output routes for midi track");
        rl = track->outRoutes();
        for (ciRoute r = rl->begin(); r != rl->end(); ++r)
        {
            //printf("Song::removeTrackRealtime %s out route port:%d\n", track->name().toLatin1().constData(), r->midiPort);
            Route src(track, r->channel);
            midiPorts[r->midiPort].inRoutes()->removeRoute(src);
        }
    }
}

//---------------------------------------------------------
//   executeScript
//---------------------------------------------------------

void Song::executeScript(const char* scriptfile, PartList* parts, int quant, bool onlyIfSelected)
{
    // a simple format for external processing
    // will be extended if there is a need
    //
    // Semantics:
    // PARTLEN <len in ticks>
    // BEATLEN <len in ticks>
    // QUANTLEN <len in ticks>
    // NOTE <tick> <nr> <len in ticks> <velocity>
    // CONTROLLER <tick> <a> <b> <c>
    //
    song->startUndo(); // undo this entire block
    for (iPart i = parts->begin(); i != parts->end(); i++)
    {
        //const char* tmp = tmpnam(NULL);
        char tmp[16] = "los-tmp-XXXXXX";
        int fd = mkstemp(tmp);
        printf("script input filename=%s\n", tmp);
        //FILE *fp = fopen(tmp, "w");
        FILE *fp = fdopen(fd, "w");
        MidiPart *part = (MidiPart*) (i->second);
        int partStart = part->endTick() - part->lenTick();
        int z, n;
        sigmap.timesig(0, z, n);
        fprintf(fp, "TIMESIG %d %d\n", z, n);
        fprintf(fp, "PART %d %d\n", partStart, part->lenTick());
        fprintf(fp, "BEATLEN %d\n", sigmap.ticksBeat(0));
        fprintf(fp, "QUANTLEN %d\n", quant);

        //for (iCItem i = items.begin(); i != items.end(); ++i) {
        for (iEvent e = part->events()->begin(); e != part->events()->end(); e++)
        {
            Event ev = e->second;

            if (ev.isNote())
            {
                if (onlyIfSelected && ev.selected() == false)
                    continue;

                fprintf(fp, "NOTE %d %d %d %d\n", ev.tick(), ev.dataA(), ev.lenTick(), ev.dataB());
                // Indicate no undo, and do not do port controller values and clone parts.
                audio->msgDeleteEvent(ev, part, false, false, false);
            }
            else if (ev.type() == Controller)
            {
                fprintf(fp, "CONTROLLER %d %d %d %d\n", ev.tick(), ev.dataA(), ev.dataB(), ev.dataC());
                // Indicate no undo, and do not do port controller values and clone parts.
                audio->msgDeleteEvent(ev, part, false, false, false);
            }
        }
        fclose(fp);

        // Call external program, let it manipulate the file
        int pid = fork();
        if (pid == 0)
        {
            if (execlp(scriptfile, scriptfile, tmp, NULL) == -1)
            {
                perror("Failed to launch script!");
                // Get out of here

                // cannot report error through gui, we are in another fork!
                //@!TODO: Handle unsuccessful attempts
                exit(99);
            }
            exit(0);
        }
        else if (pid == -1)
        {
            perror("fork failed");
        }
        else
        {
            int status;
            waitpid(pid, &status, 0);
            if (WEXITSTATUS(status) != 0)
            {
                QMessageBox::warning(los, tr("LOS - external script failed"),
                        tr("LOS was unable to launch the script\n")
                        );
                endUndo(SC_EVENT_REMOVED);
                return;
            }
            else
            { // d0 the fun55or5!
                // TODO: Create a new part, update the entire editor from it, hehh....

                QFile file(tmp);
                if (file.open(QIODevice::ReadOnly))
                {
                    QTextStream stream(&file);
                    QString line;
                    while (!stream.atEnd())
                    {
                        line = stream.readLine(); // line of text excluding '\n'
                        if (line.startsWith("NOTE"))
                        {
                            QStringList sl = line.split(" ");

                            Event e(Note);
                            int tick = sl[1].toInt();
                            int pitch = sl[2].toInt();
                            int len = sl[3].toInt();
                            int velo = sl[4].toInt();
                            printf("tick=%d pitch=%d velo=%d len=%d\n", tick, pitch, velo, len);
                            e.setTick(tick);
                            e.setPitch(pitch);
                            e.setVelo(velo);
                            e.setLenTick(len);
                            // Indicate no undo, and do not do port controller values and clone parts.
                            audio->msgAddEvent(e, part, false, false, false);
                        }
                        if (line.startsWith("CONTROLLER"))
                        {
                            QStringList sl = line.split(" ");

                            Event e(Controller);
                            int tick = sl[1].toInt();
                            int a = sl[2].toInt();
                            int b = sl[3].toInt();
                            int c = sl[4].toInt();
                            printf("tick=%d a=%d b=%d c=%d\n", tick, a, b, c);
                            e.setA(a);
                            e.setB(b);
                            e.setB(c);
                            // Indicate no undo, and do not do port controller values and clone parts.
                            audio->msgAddEvent(e, part, false, false, false);
                        }
                    }
                    file.close();
                }
            }
        }
        remove(tmp);
    }

    endUndo(SC_EVENT_REMOVED);
}

void Song::populateScriptMenu(QMenu* menuPlugins, QObject* receiver)
{
    //
    // List scripts
    //
    QString distScripts = losGlobalShare + "/scripts";

    QString userScripts = configPath + "/scripts";

    QFileInfo distScriptsFi(distScripts);
    if (distScriptsFi.isDir())
    {
        QDir dir = QDir(distScripts);
        dir.setFilter(QDir::Executable | QDir::Files);
        deliveredScriptNames = dir.entryList();
    }
    QFileInfo userScriptsFi(userScripts);
    if (userScriptsFi.isDir())
    {
        QDir dir(userScripts);
        dir.setFilter(QDir::Executable | QDir::Files);
        userScriptNames = dir.entryList();
    }

    QSignalMapper* distSignalMapper = new QSignalMapper(this);
    QSignalMapper* userSignalMapper = new QSignalMapper(this);

    if (deliveredScriptNames.size() > 0 || userScriptNames.size() > 0)
    {
        //menuPlugins = new QPopupMenu(this);
        //menuBar()->insertItem(tr("&Plugins"), menuPlugins);
        int id = 0;
        if (deliveredScriptNames.size() > 0)
        {
            for (QStringList::Iterator it = deliveredScriptNames.begin(); it != deliveredScriptNames.end(); it++, id++)
            {
                //menuPlugins->insertItem(*it, this, SLOT(execDeliveredScript(int)), 0, id);
                //menuPlugins->insertItem(*it, this, slot_deliveredscripts, 0, id);
                QAction* act = menuPlugins->addAction(*it);
                connect(act, SIGNAL(triggered()), distSignalMapper, SLOT(map()));
                distSignalMapper->setMapping(act, id);
            }
            menuPlugins->addSeparator();
        }
        if (userScriptNames.size() > 0)
        {
            for (QStringList::Iterator it = userScriptNames.begin(); it != userScriptNames.end(); it++, id++)
            {
                //menuPlugins->insertItem(*it, this, slot_userscripts, 0, id);
                QAction* act = menuPlugins->addAction(*it);
                connect(act, SIGNAL(triggered()), userSignalMapper, SLOT(map()));
                userSignalMapper->setMapping(act, id);
            }
            menuPlugins->addSeparator();
        }
        connect(distSignalMapper, SIGNAL(mapped(int)), receiver, SLOT(execDeliveredScript(int)));
        connect(userSignalMapper, SIGNAL(mapped(int)), receiver, SLOT(execUserScript(int)));
    }
    return;
}

//---------------------------------------------------------
//   getScriptPath
//---------------------------------------------------------

QString Song::getScriptPath(int id, bool isdelivered)
{
    if (isdelivered)
    {
        QString path = losGlobalShare + "/scripts/" + deliveredScriptNames[id];
        return path;
    }

    QString path = configPath + "/scripts/" + userScriptNames[id - deliveredScriptNames.size()];
    return path;
}


MidiTrackList Song::getSelectedTracks()
{
    MidiTrackList list;

    for (iMidiTrack t = _viewtracks.begin(); t != _viewtracks.end(); ++t)
    {
        MidiTrack* tr = *t;
        if (tr->selected())
        {
            list.push_back(tr);
        }
    }

    return list;
}

void Song::setTrackHeights(MidiTrackList &list, int height)
{
        for (iMidiTrack t = list.begin(); t != list.end(); ++t)
        {
                Track* tr = *t;
                tr->setHeight(height);
        }

        song->update(SC_TRACK_MODIFIED);
}

void Song::toggleFeedback(bool f)
{
    midiMonitor->msgToggleFeedback(f);
}

void Song::movePlaybackToPart(Part* part)/*{{{*/
{
    bool snap = tconfig().get_property("PianorollEdit", "snaptopart", true).toBool();
    if(audio->isPlaying() || !snap)
        return;
    if(part)
    {
        unsigned tick = part->tick();
        EventList* el = part->events();
        if(el->empty())
        {//move pb to part start
            Pos p(tick, true);
            song->setPos(0, p, true, true, true);
        }
        else
        {
            for(iEvent i = el->begin(); i != el->end(); ++i)
            {
                Event ev = i->second;
                if(ev.isNote())
                {
                    Pos p(tick+ev.tick(), true);
                    song->setPos(0, p, true, true, true);
                    break;
                }
            }
        }
    }
}/*}}}*/

QList<Part*> Song::selectedParts()
{
    QList<Part*> selected;
    for (iMidiTrack it = _viewtracks.begin(); it != _viewtracks.end(); ++it)
    {
        PartList* pl = (*it)->parts();
        for (iPart ip = pl->begin(); ip != pl->end(); ++ip)
        {
            if (ip->second->selected())
            {
                selected.append(ip->second);
            }
        }
    }
    return selected;
}
