//===========================================================
//  LOS
//  Libre Octave Studio
//  (C) Copyright 2011 Andrew Williams & Christopher Cherrett
//===========================================================

#include "TrackManager.h"
#include "globaldefs.h"
#include "globals.h"
#include "gconfig.h"
#include "track.h"
#include "song.h"
#include "app.h"
#include "mididev.h"
#include "midiport.h"
#include "minstrument.h"
#include "audio.h"
#include "midiseq.h"
#include "driver/jackaudio.h"
#include "driver/jackmidi.h"
#include "driver/alsamidi.h"
#include "icons.h"
#include "midimonitor.h"
#include "xml.h"
#include "utils.h"

#include "QMessageBox"

VirtualTrack::VirtualTrack()
{/*{{{*/
    id = create_id();
    type = -1;
    useOutput = false;
    useInput = false;
    useGlobalInputs = false;
    inputChannel = 0;
    outputChannel = 0;
    createMidiInputDevice = false;
    createMidiOutputDevice = false;
}/*}}}*/

void VirtualTrack::write(int level, Xml& xml) const/*{{{*/
{
    QStringList tmplist;
    std::string tag = "virtualTrack";
    xml.nput(level, "<%s id=\"%lld\" ", tag.c_str(), id);
    level++;
    xml.nput("type=\"%d\" useGlobalInputs=\"%d\" name=\"%s\" useInput=\"%d\" useOutput=\"%d\" ", type, useGlobalInputs, name.toUtf8().constData(), useInput, useOutput);
    if(!instrumentName.isEmpty() && type == Track::MIDI)
    {
        xml.nput("createMidiInputDevice=\"%d\" createMidiOutputDevice=\"%d\" ",
                createMidiInputDevice, createMidiOutputDevice);
        xml.nput("instrumentName=\"%s\" ",
                instrumentName.toUtf8().constData());
        xml.nput("inputChannel=\"%d\" outputChannel=\"%d\" ",
                inputChannel, outputChannel);
    }
    if(useInput && !useGlobalInputs)
    {
        tmplist.clear();
        tmplist << QString::number(inputConfig.first) << inputConfig.second;
        xml.nput("inputConfig=\"%s\" ", tmplist.join("@-:-@").toUtf8().constData());
    }
    if(useOutput)
    {
        tmplist.clear();
        tmplist << QString::number(outputConfig.first) << outputConfig.second;
        xml.nput("outputConfig=\"%s\" ", tmplist.join("@-:-@").toUtf8().constData());
    }
    xml.put("/>");
    level--;
}/*}}}*/

void VirtualTrack::read(Xml &xml)/*{{{*/
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
                break;
            case Xml::Attribut:
                if(tag == "id")
                {
                    id = xml.s2().toLongLong();
                }
                else if(tag == "name")
                {
                    name = xml.s2();
                }
                else if(tag == "type")
                {
                    type = xml.s2().toInt();
                }
                else if(tag == "instrumentName")
                {
                    instrumentName = xml.s2();
                }
                else if(tag == "createMidiInputDevice")
                {
                    createMidiInputDevice = xml.s2().toInt();
                }
                else if(tag == "createMidiOutputDevice")
                {
                    createMidiOutputDevice = xml.s2().toInt();
                }
                else if(tag == "useGlobalInputs")
                {
                    useGlobalInputs = xml.s2().toInt();
                }
                else if(tag == "useInput")
                {
                    useInput = xml.s2().toInt();
                }
                else if(tag == "useOutput")
                {
                    useOutput = xml.s2().toInt();
                }
                else if(tag == "inputChannel")
                {
                    inputChannel = xml.s2().toInt();
                }
                else if(tag == "outputChannel")
                {
                    outputChannel = xml.s2().toInt();
                }
                else if(tag == "inputConfig")
                {
                    QStringList list = xml.s2().split("@-:-@");
                    if(list.size() && list.size() == 2)
                        inputConfig = qMakePair(list[0].toInt(), list[1]);
                }
                else if(tag == "outputConfig")
                {
                    QStringList list = xml.s2().split("@-:-@");
                    if(list.size() && list.size() == 2)
                        outputConfig = qMakePair(list[0].toInt(), list[1]);
                }
                break;
            case Xml::TagEnd:
                if(tag == "virtualTrack")
                    return;
            default:
                break;
        }
    }
}/*}}}*/

TrackManager::TrackManager()
{
    m_insertPosition = -1;
    m_midiInPort = -1;
    m_midiOutPort = -1;
    m_track = 0;

    m_allChannelBit = (1 << MIDI_CHANNELS) - 1;
}

void TrackManager::setPosition(int v)
{
    m_insertPosition = v;
}

//Add button slot
qint64 TrackManager::addTrack(VirtualTrack* vtrack, int index)/*{{{*/
{
    m_insertPosition = index;
    qint64 rv = 0;
    if(!vtrack || vtrack->name.isEmpty())
        return rv;

    Track::TrackType type = (Track::TrackType)vtrack->type;
    switch(type)
    {
        case Track::MIDI:
        {
            //Load up the instrument first
            song->startUndo();
            m_track =  song->addTrackByName(vtrack->name, Track::MIDI, m_insertPosition, false);
            if(m_track)
            {
                m_track->setMasterFlag(true);
                //if(vtrack->instrumentType == SYNTH_INSTRUMENT)
                //m_track->setHeight(MIN_TRACKHEIGHT);

                song->undoOp(UndoOp::AddTrack, m_insertPosition, m_track);
                MidiTrack* mtrack = (MidiTrack*)m_track;
                //Process Input connections
                if(vtrack->useInput)
                {
                    if(vtrack->useGlobalInputs)
                    {
                        //Get the current port(s) linked with the global inputs
                        for(int i = 0; i < gInputListPorts.size(); ++i)
                        {
                            MidiDevice* indev = 0;
                            m_midiInPort = gInputListPorts.at(i);
                            if(debugMsg)
                                qDebug("TrackManager::addTrack: Using global MIDI Input port: %i", m_midiInPort);

                            MidiPort* inport = &midiPorts[m_midiInPort];
                            if(inport)
                                indev = inport->device();
                            if(inport && indev)
                            {
                                if(debugMsg)
                                    qDebug("TrackManager::addTrack: MIDI Input port and MIDI devices found, Adding final input routing");
                                int chanbit = vtrack->inputChannel;
                                Route srcRoute(m_midiInPort, chanbit);
                                Route dstRoute(m_track, chanbit);

                                audio->msgAddRoute(srcRoute, dstRoute);

                                audio->msgUpdateSoloStates();
                                song->update(SC_ROUTE);
                            }
                        }
                    }
                    else
                    {
                        QString devname = vtrack->inputConfig.second;
                        MidiPort* inport = 0;
                        MidiDevice* indev = 0;
                        QString inputDevName(QString("I-").append(m_track->name()));
                        if(vtrack->createMidiInputDevice)
                        {
                            m_midiInPort = getFreeMidiPort();
                            if(debugMsg)
                                qDebug("TrackManager::addTrack: createMidiInputDevice is set: %i", m_midiInPort);
                            inport = &midiPorts[m_midiInPort];
                            int devtype = vtrack->inputConfig.first;
                            if(devtype == MidiDevice::ALSA_MIDI)
                            {
                                indev = midiDevices.find(devname, MidiDevice::ALSA_MIDI);
                                if(indev)
                                {
                                    if(debugMsg)
                                        qDebug("TrackManager::addTrack: Found MIDI input device: ALSA_MIDI");
                                    int openFlags = 0;
                                    openFlags ^= 0x2;
                                    indev->setOpenFlags(openFlags);
                                    midiSeq->msgSetMidiDevice(inport, indev);
                                }
                            }
                            else if(devtype == MidiDevice::JACK_MIDI)
                            {
                                indev = MidiJackDevice::createJackMidiDevice(inputDevName, 3);
                                if(indev)
                                {
                                    if(debugMsg)
                                        qDebug("TrackManager::addTrack: Created MIDI input device: JACK_MIDI");
                                    int openFlags = 0;
                                    openFlags ^= 0x2;
                                    indev->setOpenFlags(openFlags);
                                    midiSeq->msgSetMidiDevice(inport, indev);
                                }
                            }
                            if(indev && indev->deviceType() == MidiDevice::JACK_MIDI)
                            {
                                if(debugMsg)
                                    qDebug("TrackManager::addTrack: MIDI input device configured, Adding input routes to MIDI port");
                                Route srcRoute(devname, false, -1, Route::JACK_ROUTE);
                                Route dstRoute(indev, -1);

                                audio->msgAddRoute(srcRoute, dstRoute);

                                audio->msgUpdateSoloStates();
                                song->update(SC_ROUTE);
                            }
                        }
                        else
                        {
                            m_midiInPort = vtrack->inputConfig.first;
                            if(debugMsg)
                                qDebug("TrackManager::addTrack: Using existing MIDI port: %i", m_midiInPort);
                            inport = &midiPorts[m_midiInPort];
                            if(inport)
                                indev = inport->device();
                        }
                        if(inport && indev)
                        {
                            if(debugMsg)
                                qDebug("TrackManager::addTrack: MIDI Input port and MIDI devices found, Adding final input routing");
                            int chanbit = vtrack->inputChannel;
                            Route srcRoute(m_midiInPort, chanbit);
                            Route dstRoute(m_track, chanbit);

                            audio->msgAddRoute(srcRoute, dstRoute);

                            audio->msgUpdateSoloStates();
                            song->update(SC_ROUTE);
                        }
                    }
                }

                //Process Output connections
                if(vtrack->useOutput)
                {
                    MidiPort* outport= 0;
                    MidiDevice* outdev = 0;
                    QString devname = vtrack->outputConfig.second;
                    QString outputDevName(QString("O-").append(m_track->name()));
                    if(vtrack->createMidiOutputDevice)
                    {
                        m_midiOutPort = getFreeMidiPort();
                        if(debugMsg)
                            qDebug("TrackManager::addTrack: m_createMidiOutputDevice is set: %i", m_midiOutPort);
                        outport = &midiPorts[m_midiOutPort];
                        int devtype = vtrack->outputConfig.first;
                        if(devtype == MidiDevice::ALSA_MIDI)
                        {
                            outdev = midiDevices.find(devname, MidiDevice::ALSA_MIDI);
                            if(outdev)
                            {
                                if(debugMsg)
                                    qDebug("TrackManager::addTrack: Found MIDI output device: ALSA_MIDI");
                                int openFlags = 0;
                                openFlags ^= 0x1;
                                outdev->setOpenFlags(openFlags);
                                midiSeq->msgSetMidiDevice(outport, outdev);
                            }
                        }
                        else if(devtype == MidiDevice::JACK_MIDI)
                        {
                            outdev = MidiJackDevice::createJackMidiDevice(outputDevName, 3);
                            if(outdev)
                            {
                                if(debugMsg)
                                    qDebug("TrackManager::addTrack: Created MIDI output device: JACK_MIDI");
                                int openFlags = 0;
                                openFlags ^= 0x1;
                                outdev->setOpenFlags(openFlags);
                                midiSeq->msgSetMidiDevice(outport, outdev);
                                los->changeConfig(true);
                                song->update();
                            }
                        }
                        if(outdev && outdev->deviceType() == MidiDevice::JACK_MIDI)
                        {
                            if(debugMsg)
                                qDebug("TrackManager::addTrack: MIDI output device configured, Adding output routes to MIDI port");
                            Route srcRoute(outdev, -1);
                            Route dstRoute(devname, true, -1, Route::JACK_ROUTE);

                            if(debugMsg)
                                qDebug("TrackManager::addTrack: Device name from combo: %s, from dev: %s", devname.toUtf8().constData(), outdev->name().toUtf8().constData());

                            audio->msgAddRoute(srcRoute, dstRoute);

                            audio->msgUpdateSoloStates();
                            song->update(SC_ROUTE);
                        }
                    }
                    else
                    {
                        m_midiOutPort = vtrack->outputConfig.first;
                        if(debugMsg)
                            qDebug("TrackManager::addTrack: Using existing MIDI output port: %i", m_midiOutPort);
                        outport = &midiPorts[m_midiOutPort];
                        if(outport)
                            outdev = outport->device();
                    }
                    if(outport && outdev)
                    {
                        if(debugMsg)
                            qDebug("TrackManager::addTrack: MIDI output port and MIDI devices found, Adding final output routing");
                        audio->msgIdle(true);
                        mtrack->setOutPortAndUpdate(m_midiOutPort);
                        int outChan = vtrack->outputChannel;
                        mtrack->setOutChanAndUpdate(outChan);
                        audio->msgIdle(false);
                        if(vtrack->createMidiOutputDevice)
                        {
                            QString instrumentName = vtrack->instrumentName;
                            for (iMidiInstrument i = midiInstruments.begin(); i != midiInstruments.end(); ++i)
                            {
                                if ((*i)->iname() == instrumentName)
                                {
                                    outport->setInstrument(*i);
                                    break;
                                }
                            }
                        }
                        song->update(SC_MIDI_TRACK_PROP);
                    }
                }

                song->deselectTracks();
                m_track->setSelected(true);
                emit trackAdded(m_track->id());
                rv = m_track->id();
            }
            song->endUndo(SC_TRACK_INSERTED | SC_TRACK_MODIFIED);
            song->updateTrackViews();
        }
        break;
        case Track::WAVE:
        {
            song->startUndo();
            m_track =  song->addTrackByName(vtrack->name, Track::WAVE, m_insertPosition, false);
            if(m_track)
            {
                m_track->setMasterFlag(true);
                song->undoOp(UndoOp::AddTrack, m_insertPosition, m_track);

                if(vtrack->useInput)
                {
                    if (Track* input = m_track->inputTrack())
                    {
                        QString inputName = input->name();
                        QString selectedInput = vtrack->inputConfig.second;
                        //bool addNewRoute = vtrack->inputConfig.first;

                        song->undoOp(UndoOp::AddTrack, -1, input);

                        //if(addNewRoute)
                        {
                            input->setMute(false);
                            //Connect jack port to Input track
                            Route srcRoute(selectedInput, false, -1, Route::JACK_ROUTE);
                            Route dstRoute(input, 0);
                            srcRoute.channel = 0;

                            Route srcRoute2(selectedInput, false, -1, Route::JACK_ROUTE);
                            Route dstRoute2(input, 1);
                            srcRoute2.channel = 1;

                            audio->msgAddRoute(srcRoute, dstRoute);
                            audio->msgAddRoute(srcRoute2, dstRoute2);

                            audio->msgUpdateSoloStates();
                            song->update(SC_ROUTE);
                        }
                    }
                }

                if(vtrack->useOutput)
                {
                    if (Track* output = m_track->outputTrack())
                    {
                        QString jackPlayback("system:playback");
                        QString selectedOutput = vtrack->outputConfig.second;
                        //bool addNewRoute = vtrack->outputConfig.first;

                        song->undoOp(UndoOp::AddTrack, -1, output);

                        //if(addNewRoute)
                        {
                            if (selectedOutput.startsWith(jackPlayback))
                            {
                                //Route channel 1
                                Route srcRoute(output, 0);
                                Route dstRoute(QString(jackPlayback).append("_1"), true, -1, Route::JACK_ROUTE);
                                dstRoute.channel = 0;

                                //Route channel 2
                                Route srcRoute2(output, 1);
                                Route dstRoute2(QString(jackPlayback).append("_2"), true, -1, Route::JACK_ROUTE);
                                dstRoute2.channel = 1;

                                audio->msgAddRoute(srcRoute, dstRoute);
                                audio->msgAddRoute(srcRoute2, dstRoute2);
                            }
                            else
                            {
                                //Route channel 1
                                Route srcRoute(output, 0);
                                Route dstRoute(selectedOutput, true, -1, Route::JACK_ROUTE);
                                dstRoute.channel = 0;

                                //Route channel 2
                                Route srcRoute2(output, 1);
                                Route dstRoute2(selectedOutput, true, -1, Route::JACK_ROUTE);
                                dstRoute2.channel = 1;

                                audio->msgAddRoute(srcRoute, dstRoute);
                                audio->msgAddRoute(srcRoute2, dstRoute2);
                            }
                            audio->msgUpdateSoloStates();
                            song->update(SC_ROUTE);
                        }
                    }
                }

                // always output wave tracks to master
                Route srcRoute(m_track, 0, m_track->channels());
                Route dstRoute(song->masterId(), true, -1);
                audio->msgAddRoute(srcRoute, dstRoute);
                audio->msgUpdateSoloStates();
                song->update(SC_ROUTE);

                song->deselectTracks();
                m_track->setSelected(true);
                emit trackAdded(m_track->id());
                rv = m_track->id();

            }
            song->endUndo(SC_TRACK_INSERTED | SC_TRACK_MODIFIED);
            song->updateTrackViews();
        }
        break;
        case Track::WAVE_OUTPUT_HELPER:
        {
        qFatal("Trying to manually add OUT track, stop!");
#if 0
            song->startUndo();
            m_track = song->addTrackByName(vtrack->name, Track::AUDIO_OUTPUT, -1, false);
            if(m_track)
            {
                m_track->setMasterFlag(true);
                song->undoOp(UndoOp::AddTrack, -1, m_track);
                if(vtrack->useInput)
                {
                    QString selectedInput = vtrack->inputConfig.second;
                    Route dstRoute(m_track, 0, m_track->channels());
                    Route srcRoute(selectedInput, true, -1);

                    audio->msgAddRoute(srcRoute, dstRoute);

                    audio->msgUpdateSoloStates();
                    song->update(SC_ROUTE);
                }

                if(vtrack->useOutput)
                {
                    QString jackPlayback("system:playback");
                    QString selectedOutput = vtrack->outputConfig.second;
                    bool systemOutput = false;
                    if(selectedOutput.startsWith(jackPlayback))
                    {//FIXME: Change this to support more than two system playback devices
                        systemOutput = true;

                        //Route channel 1
                        Route srcRoute(m_track, 0);
                        Route dstRoute(QString(jackPlayback).append("_1"), true, -1, Route::JACK_ROUTE);
                        dstRoute.channel = 0;

                        audio->msgAddRoute(srcRoute, dstRoute);

                        //Route channel 2
                        Route srcRoute2(m_track, 1);
                        Route dstRoute2(QString(jackPlayback).append("_2"), true, -1, Route::JACK_ROUTE);
                        dstRoute2.channel = 1;

                        audio->msgAddRoute(srcRoute2, dstRoute2);
                    }
                    else
                    {
                        //Route channel 1
                        Route srcRoute(m_track, 0);
                        Route dstRoute(selectedOutput, true, -1, Route::JACK_ROUTE);
                        dstRoute.channel = 0;

                        audio->msgAddRoute(srcRoute, dstRoute);

                        //Route channel 2
                        Route srcRoute2(m_track, 1);
                        Route dstRoute2(selectedOutput, true, -1, Route::JACK_ROUTE);
                        dstRoute2.channel = 1;

                        audio->msgAddRoute(srcRoute2, dstRoute2);
                    }

                    audio->msgUpdateSoloStates();
                    song->update(SC_ROUTE);
                }
                song->deselectTracks();
                m_track->setSelected(true);
                emit trackAdded(m_track->id());
                rv = m_track->id();
            }
            song->endUndo(SC_TRACK_INSERTED | SC_TRACK_MODIFIED);
            song->updateTrackViews();
#endif
        }
        break;
        case Track::WAVE_INPUT_HELPER:
        {
        qFatal("Trying to manually add IN track, stop!");
#if 0
            song->startUndo();
            m_track = song->addTrackByName(vtrack->name, Track::AUDIO_INPUT, -1, false);
            if(m_track)
            {
                m_track->setMasterFlag(true);
                song->undoOp(UndoOp::AddTrack, -1, m_track);
                m_track->setMute(false);
                if(vtrack->useInput)
                {
                    QString selectedInput = vtrack->inputConfig.second;

                    QString jackCapture("system:capture");
                    if(selectedInput.startsWith(jackCapture))
                    {
                        Route srcRoute(QString(jackCapture).append("_1"), false, -1, Route::JACK_ROUTE);
                        Route dstRoute(m_track, 0);
                        srcRoute.channel = 0;
                        audio->msgAddRoute(srcRoute, dstRoute);

                        Route srcRoute2(QString(jackCapture).append("_2"), false, -1, Route::JACK_ROUTE);
                        Route dstRoute2(m_track, 1);
                        srcRoute2.channel = 1;
                        audio->msgAddRoute(srcRoute2, dstRoute2);
                    }
                    else
                    {
                        Route srcRoute(selectedInput, false, -1, Route::JACK_ROUTE);
                        Route dstRoute(m_track, 0);
                        srcRoute.channel = 0;
                        audio->msgAddRoute(srcRoute, dstRoute);

                        Route srcRoute2(selectedInput, false, -1, Route::JACK_ROUTE);
                        Route dstRoute2(m_track, 1);
                        srcRoute2.channel = 1;
                        audio->msgAddRoute(srcRoute2, dstRoute2);
                    }

                    audio->msgUpdateSoloStates();
                    song->update(SC_ROUTE);
                }
                if(vtrack->useOutput)
                {
                    QString selectedOutput = vtrack->outputConfig.second;

                    Route srcRoute(m_track, 0, m_track->channels());
                    Route dstRoute(selectedOutput, true, -1);

                    audio->msgAddRoute(srcRoute, dstRoute);
                    audio->msgUpdateSoloStates();
                    song->update(SC_ROUTE);
                }
                song->deselectTracks();
                m_track->setSelected(true);
                emit trackAdded(m_track->id());
                rv = m_track->id();
            }
            song->endUndo(SC_TRACK_INSERTED | SC_TRACK_MODIFIED);
            song->updateTrackViews();
#endif
        }
        break;
        default:
        break;
    }
    m_track = 0;
    return rv;
}/*}}}*/

void TrackManager::setTrackInstrument(qint64 tid, const QString& instrument, int type)/*{{{*/
{
    Track *t = song->findTrackById(tid);

    if(!t || instrument.isEmpty())
        return;
    m_track = t;
    MidiTrack* track = (MidiTrack*) m_track;
    MidiPort *mp = losMidiPorts.value(track->outPortId());
    if(mp)
    {
        MidiInstrument* oldins = mp->instrument();
        if(!oldins || oldins->iname() == instrument)
            return;//No change

        {//External manual cannections required
            MidiInstrument* ins = 0;
            for (iMidiInstrument i = midiInstruments.begin(); i != midiInstruments.end(); ++i)
            {
                if ((*i)->iname() == instrument)
                {
                    ins = *i;
                    break;
                }
            }
            if(!ins)
            {//Reset GUI widgets that made this call and return
                return;
            }
            mp->setInstrument(ins);

            track->setSamplerData(0);
            track->setWantsAutomation(false);
            midiSeq->msgSetMidiDevice(mp, 0);
            song->update(SC_MIDI_TRACK_PROP);
        }
        song->update(SC_MIDI_TRACK_PROP);
        song->update(SC_TRACK_INSTRUMENT);
        song->dirty = true;
    }
}/*}}}*/

void TrackManager::removeTrack(qint64 id)/*{{{*/
{
    Track* track = song->findTrackById(id);
    if(track)
    {
        QList<qint64> idList;
        QStringList names;
        if(track->masterFlag())
        {//Find children
            idList.append(track->id());
            names.append(track->name());
            if(track->hasChildren())
            {
                QList<qint64> *chain = track->audioChain();
                for(int i = 0; i < chain->size(); i++)
                {
                    Track* child = song->findTrackById(chain->at(i));
                    if(child && child->chainMaster() == id)
                    {
                        names.append(child->name());
                        idList.append(child->id());
                    }
                }
            }
        }
        else
        {//Find parent and siblings
            Track *ptrack = song->findTrackById(track->chainMaster());
            if(ptrack && ptrack->chainContains(id))
            {//We're good
                idList.append(ptrack->id());
                names.append(ptrack->name());
                if(ptrack->hasChildren())
                {
                    QList<qint64> *chain = ptrack->audioChain();
                    for(int i = 0; i < chain->size(); i++)
                    {
                        Track* child = song->findTrackById(chain->at(i));
                        if(child)
                        {
                            names.append(child->name());
                            idList.append(child->id());
                        }
                    }
                }
            }
            else
            {//Unparented track, maybe old file, just delete it
                idList.append(track->id());
                names.append(track->name());
            }
        }
        //Process list
        if(idList.size())
        {
            QString msg(tr("You are about to delete the following track(s) \n%1 \nAre you sure this is what you want?"));
            if(QMessageBox::question(los,
                tr("Delete Track(s)"),
                msg.arg(names.join("\n")),
                QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
            {
                audio->msgRemoveTrackGroup(idList, true);
            }
        }
    }
}/*}}}*/

void TrackManager::removeSelectedTracks()/*{{{*/
{
    QList<qint64> selected = song->selectedTracks();
    if(selected.isEmpty())
        return;
    QList<qint64> idList;
    QStringList names;
    for(int t = 0; t < selected.size(); t++)
    {
        qint64 id =  selected.at(t);
        Track* track = song->findTrackById(id);
        if(track)
        {
            if(track->masterFlag())
            {//Find children
                if(!idList.contains(track->id()))
                    idList.append(track->id());
                if(!names.contains(track->name()))
                    names.append(track->name());
                if(track->hasChildren())
                {
                    QList<qint64> *chain = track->audioChain();
                    for(int i = 0; i < chain->size(); i++)
                    {
                        Track* child = song->findTrackById(chain->at(i));
                        if(child && child->chainMaster() == id)
                        {
                            if(!names.contains(child->name()))
                                names.append(child->name());
                            if(!idList.contains(child->id()))
                                idList.append(child->id());
                        }
                    }
                }
            }
            else
            {//Find parent and siblings
                Track *ptrack = song->findTrackById(track->chainMaster());
                if(ptrack && ptrack->chainContains(id))
                {//We're good
                    if(!idList.contains(ptrack->id()))
                        idList.append(ptrack->id());
                    if(!names.contains(ptrack->name()))
                        names.append(ptrack->name());
                    if(ptrack->hasChildren())
                    {
                        QList<qint64> *chain = ptrack->audioChain();
                        for(int i = 0; i < chain->size(); i++)
                        {
                            Track* child = song->findTrackById(chain->at(i));
                            if(child)
                            {
                                if(!names.contains(child->name()))
                                    names.append(child->name());
                                if(!idList.contains(child->id()))
                                    idList.append(child->id());
                            }
                        }
                    }
                }
                else
                {//Unparented track, maybe old file, just delete it
                    if(!idList.contains(track->id()))
                        idList.append(track->id());
                    if(!names.contains(track->name()))
                        names.append(track->name());
                }
            }
        }
    }
    //Process list
    if(idList.size())
    {
        QString msg(tr("You are about to delete the following track(s) \n%1 \nAre you sure this is what you want?"));
        if(QMessageBox::question(los,
            tr("Delete Track(s)"),
            msg.arg(names.join("\n")),
            QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
        {
            audio->msgRemoveTrackGroup(idList, true);
        }
    }
}/*}}}*/

bool TrackManager::removeTrack(VirtualTrack* vtrack)/*{{{*/
{
    bool rv = false;
    Q_UNUSED(vtrack);
    return rv;
}/*}}}*/

void TrackManager::write(int level, Xml& xml) const/*{{{*/
{
    std::string tag = "trackManager";
    xml.put(level, "<%s tracks=\"%d\">",tag.c_str() , m_virtualTracks.size());
    level++;
    foreach(VirtualTrack* vt, m_virtualTracks)
    {
        vt->write(level, xml);
    }
    xml.put(--level, "</%s>", tag.c_str());
}/*}}}*/

void TrackManager::read(Xml& xml)/*{{{*/
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
                if(tag == "virtualTrack")
                {
                    VirtualTrack* vt = new VirtualTrack;
                    vt->read(xml);
                    m_virtualTracks.insert(vt->id, vt);
                }
                break;
            case Xml::Attribut:
                if(tag == "tracks")
                {//We'll do something with this later
                    xml.s2().toInt();
                }
                break;
            case Xml::TagEnd:
                if(tag == "trackManager")
                    return;
            default:
                break;
        }
    }
}/*}}}*/

