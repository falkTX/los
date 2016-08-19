//=========================================================
//  LOS
//  Libre Octave Studio
//  $Id: seqmsg.cpp,v 1.32.2.17 2009/12/20 05:00:35 terminator356 Exp $
//
//  (C) Copyright 2001 Werner Schweer (ws@seh.de)
//=========================================================

#include <stdio.h>

#include "song.h"
#include "midiport.h"
#include "minstrument.h"
#include "app.h"
#include "tempo.h"
#include "globals.h"
#include "sig.h"
#include "audio.h"
#include "mididev.h"
#include "audiodev.h"
#include "alsamidi.h"
#include "audio.h"
#include "Arranger/Arranger.h"
#include "driver/jackmidi.h"

//---------------------------------------------------------
//   sendMsg
//---------------------------------------------------------

void Audio::sendMsg(AudioMsg* m, bool waitRead)
{
    static int sno = 0;

    if (_running && waitRead)
    {
        m->serialNo = sno++;
        //DEBUG:
        msg = m;
        // wait for next audio "process" call to finish operation
        int no = -1;
        int rv = read(fromThreadFdr, &no, sizeof (int));
        if (rv != sizeof (int))
            perror("Audio: read pipe failed");
        else if (no != (sno - 1))
        {
            fprintf(stderr, "audio: bad serial number, read %d expected %d\n",
                    no, sno - 1);
        }
    }
    else
    {
        // if audio is not running (during initialization)
        // process commands immediatly
        processMsg(m);
    }
}

//---------------------------------------------------------
//   sendMessage
//    send request from gui to sequencer
//    wait until request is processed
//---------------------------------------------------------

bool Audio::sendMessage(AudioMsg* m, bool doUndo, bool waitRead)
{
    if (doUndo)
        song->startUndo();

    if (waitRead)
        sendMsg(m);
    else
        processMsg(m);

    if (doUndo)
        song->endUndo(0);

    return false;
}

//---------------------------------------------------------
//   msgRemoveRoute
//---------------------------------------------------------

void Audio::msgRemoveRoute(Route src, Route dst)
{
    msgRemoveRoute1(src, dst);
    if (src.type == Route::JACK_ROUTE)
    {
        if (!checkAudioDevice()) return;

        if (dst.type == Route::MIDI_DEVICE_ROUTE)
        {
            if (dst.device)
            {
                if (dst.device->deviceType() == MidiDevice::JACK_MIDI)
                    audioDevice->disconnect(src.jackPort, dst.device->inClientPort()); // p3.3.55
            }
        }
        //else
        //    audioDevice->disconnect(src.jackPort, ((AudioInputHelper*) dst.track)->jackPort(dst.channel));
    }
    else if (dst.type == Route::JACK_ROUTE)
    {
        if (!checkAudioDevice()) return;

        if (src.type == Route::MIDI_DEVICE_ROUTE)
        {
            if (src.device)
            {
                if (src.device->deviceType() == MidiDevice::JACK_MIDI)
                    audioDevice->disconnect(src.device->outClientPort(), dst.jackPort); // p3.3.55
            }
        }
        //else
        //    audioDevice->disconnect(((AudioOutputHelper*) src.track)->jackPort(src.channel), dst.jackPort);
    }
}

//---------------------------------------------------------
//   msgRemoveRoute1
//---------------------------------------------------------

void Audio::msgRemoveRoute1(Route src, Route dst)
{
    AudioMsg msg;
    msg.id = AUDIO_ROUTEREMOVE;
    msg.sroute = src;
    msg.droute = dst;
    sendMsg(&msg);
}

//---------------------------------------------------------
//   msgRemoveRoutes
//---------------------------------------------------------

// p3.3.55

void Audio::msgRemoveRoutes(Route src, Route dst)
{
    msgRemoveRoutes1(src, dst);
}

//---------------------------------------------------------
//   msgRemoveRoutes1
//---------------------------------------------------------

// p3.3.55

void Audio::msgRemoveRoutes1(Route src, Route dst)
{
    AudioMsg msg;
    msg.id = AUDIO_REMOVEROUTES;
    msg.sroute = src;
    msg.droute = dst;
    sendMsg(&msg);
}

//---------------------------------------------------------
//   msgAddRoute
//---------------------------------------------------------

void Audio::msgAddRoute(Route src, Route dst)
{
    if (src.type == Route::JACK_ROUTE)
    {
        if (!checkAudioDevice()) return;
        if (isRunning())
        {
            if (dst.type == Route::MIDI_DEVICE_ROUTE)
            {
                if (dst.device)
                {
                    if (dst.device->deviceType() == MidiDevice::JACK_MIDI)
                        audioDevice->connect(src.jackPort, dst.device->inClientPort()); // p3.3.55
                }
            }
            //else
            //    audioDevice->connect(src.jackPort, ((AudioInputHelper*) dst.track)->jackPort(dst.channel));
        }
    }
    else if (dst.type == Route::JACK_ROUTE)
    {
        if (!checkAudioDevice()) return;
        if (audio->isRunning())
        {
            if (src.type == Route::MIDI_DEVICE_ROUTE)
            {
                if (src.device)
                {
                    if (src.device->deviceType() == MidiDevice::JACK_MIDI)
                        audioDevice->connect(src.device->outClientPort(), dst.jackPort); // p3.3.55
                }
            }
            //else
            //    audioDevice->connect(((AudioOutputHelper*) src.track)->jackPort(dst.channel), dst.jackPort);
        }
    }
    msgAddRoute1(src, dst);
}

//---------------------------------------------------------
//   msgAddRoute1
//---------------------------------------------------------

void Audio::msgAddRoute1(Route src, Route dst)
{
    AudioMsg msg;
    msg.id = AUDIO_ROUTEADD;
    msg.sroute = src;
    msg.droute = dst;
    sendMsg(&msg);
}

//---------------------------------------------------------
//   msgSetSolo
//---------------------------------------------------------

void Audio::msgSetSolo(MidiTrack* track, bool val)
{
    AudioMsg msg;
    msg.id = AUDIO_SET_SOLO;
    msg.track = track;
    msg.ival = int(val);
    sendMsg(&msg);
}

//---------------------------------------------------------
//   msgSetSegSize
//---------------------------------------------------------

void Audio::msgSetSegSize(int bs, int sr)
{
    AudioMsg msg;
    msg.id = AUDIO_SET_SEG_SIZE;
    msg.ival = bs;
    msg.iival = sr;
    sendMsg(&msg);
}

//---------------------------------------------------------
//   msgSeek
//---------------------------------------------------------

void Audio::msgSeek(const Pos& pos)
{
    if (!checkAudioDevice()) return;
    //audioDevice->seekTransport(pos.frame());
    // p3.3.23
    //printf("Audio::msgSeek before audioDevice->seekTransport frame:%d\n", pos.frame());
    audioDevice->seekTransport(pos);
    // p3.3.23
    //printf("Audio::msgSeek after audioDevice->seekTransport frame:%d\n", pos.frame());
}

//---------------------------------------------------------
//   msgUndo
//---------------------------------------------------------

void Audio::msgUndo()
{
    AudioMsg msg;
    msg.id = SEQM_UNDO;
    sendMsg(&msg);
}

//---------------------------------------------------------
//   msgRedo
//---------------------------------------------------------

void Audio::msgRedo()
{
    AudioMsg msg;
    msg.id = SEQM_REDO;
    sendMsg(&msg);
}

//---------------------------------------------------------
//   msgPlay
//---------------------------------------------------------

void Audio::msgPlay(bool val)
{
    if (val)
    {
        if (audioDevice)
        {
            unsigned sfr = song->cPos().frame();
            unsigned dcfr = audioDevice->getCurFrame();
            if (dcfr != sfr)
                //audioDevice->seekTransport(sfr);
                audioDevice->seekTransport(song->cPos());
            audioDevice->startTransport();
        }

    }
    else
    {
        if (audioDevice)
            audioDevice->stopTransport();
    }
}

//---------------------------------------------------------
//   msgAddTrack
//---------------------------------------------------------

void Song::msgInsertTrack(MidiTrack* track, int idx, bool doUndoFlag)
{
    AudioMsg msg;
    msg.id = SEQM_ADD_TRACK;
    msg.track = track;
    msg.ival = idx;
    if (doUndoFlag)
    {
        song->startUndo();
        undoOp(UndoOp::AddTrack, idx, track);
    }
    audio->sendMsg(&msg);
    if (doUndoFlag)
        endUndo(SC_TRACK_INSERTED);
}

//---------------------------------------------------------
//   msgRemoveTrack
//---------------------------------------------------------

void Audio::msgRemoveTrack(MidiTrack* track, bool doUndoFlag)
{
    AudioMsg msg;
    msg.id = SEQM_REMOVE_TRACK;
    msg.track = track;
    sendMessage(&msg, doUndoFlag);
}

//---------------------------------------------------------
//   msgRemoveTrackGroup
//---------------------------------------------------------

void Audio::msgRemoveTrackGroup(QList<qint64> list, bool doUndoFlag)
{
    AudioMsg msg;
    msg.id = SEQM_REMOVE_TRACK_GROUP;
    msg.list = list;
    sendMessage(&msg, doUndoFlag);
}

//---------------------------------------------------------
//   msgRemoveTracks
//    remove all selected tracks
//---------------------------------------------------------

void Audio::msgRemoveTracks()
{
    bool loop;
    do
    {
        loop = false;
        MidiTrackList* tl = song->tracks();
        for (iMidiTrack t = tl->begin(); t != tl->end(); ++t)
        {
            MidiTrack* tr = *t;
            if(tr->selected())
            {
                song->removeTrack1(tr);
                msgRemoveTrack(tr, false);
                loop = true;
                break;
            }
        }
    } while (loop);
}

//---------------------------------------------------------
//   msgChangeTrack
//    oldTrack - copy of the original track befor modification
//    newTrack - modified original track
//---------------------------------------------------------

void Audio::msgChangeTrack(MidiTrack* oldTrack, MidiTrack* newTrack, bool doUndoFlag)
{
    AudioMsg msg;
    msg.id = SEQM_CHANGE_TRACK;
    msg.p1 = oldTrack;
    msg.p2 = newTrack;
    sendMessage(&msg, doUndoFlag);
}

//---------------------------------------------------------
//   msgMoveTrack
//    move track idx1 to slot idx2
//---------------------------------------------------------

void Audio::msgMoveTrack(int idx1, int idx2, bool doUndoFlag)
{
    if (idx1 < 0 || idx2 < 0) // sanity check
        return;
    int n = song->visibletracks()->size();

    if (idx1 >= n || idx2 >= n) // sanity check
        return;
    AudioMsg msg;
    msg.id = SEQM_MOVE_TRACK;
    msg.a = idx1;
    msg.b = idx2;
    sendMessage(&msg, doUndoFlag);
}

//---------------------------------------------------------
//   msgAddPart
//---------------------------------------------------------

void Audio::msgAddPart(Part* part, bool doUndoFlag)
{
    AudioMsg msg;
    msg.id = SEQM_ADD_PART;
    msg.p1 = part;
    sendMessage(&msg, doUndoFlag);
}

//---------------------------------------------------------
//   msgRemovePart
//---------------------------------------------------------

void Audio::msgRemovePart(Part* part, bool doUndoFlag)
{
    AudioMsg msg;
    msg.id = SEQM_REMOVE_PART;
    msg.p1 = part;
    sendMessage(&msg, doUndoFlag);
}

void Audio::msgRemoveParts(QList<Part*> parts, bool doUndoFlag)
{
    AudioMsg msg;
    msg.id = SEQM_REMOVE_PART_LIST;
    msg.plist = parts;
    sendMessage(&msg, doUndoFlag);
}

//---------------------------------------------------------
//   msgRemoveParts
//    remove selected parts; return true if any part was
//    removed
//---------------------------------------------------------

bool Song::msgRemoveParts()
{
    bool loop;
    bool partSelected = false;
    do
    {
        loop = false;
        //TrackList* tl = song->tracks();
        //TODO: Check if this is used outside of the canvas part delete and fix them
        //since we should not be deleting parts we cant see.
        MidiTrackList* tl = visibletracks();/*{{{*/

        for (iMidiTrack it = tl->begin(); it != tl->end(); ++it)
        {
            PartList* pl = (*it)->parts();
            for (iPart ip = pl->begin(); ip != pl->end(); ++ip)
            {
                if (ip->second->selected())
                {
                    {
                        audio->msgRemovePart(ip->second, false);
                    }
                    loop = true;
                    partSelected = true;
                    break;
                }
            }
            if (loop)
                break;
        }/*}}}*/
    } while (loop);
    if(partSelected)
    {
        song->dirty = true;
        updateTrackViews();
    }
    return partSelected;
}

//---------------------------------------------------------
//   msgChangePart
//---------------------------------------------------------

//void Audio::msgChangePart(Part* oldPart, Part* newPart, bool doUndoFlag)

void Audio::msgChangePart(Part* oldPart, Part* newPart, bool doUndoFlag, bool doCtrls, bool doClones)
{
    AudioMsg msg;
    msg.id = SEQM_CHANGE_PART;
    msg.p1 = oldPart;
    msg.p2 = newPart;
    msg.a = doCtrls;
    msg.b = doClones;
    sendMessage(&msg, doUndoFlag);
}

//---------------------------------------------------------
//   msgAddEvent
//---------------------------------------------------------

//void Audio::msgAddEvent(Event& event, Part* part, bool doUndoFlag)

void Audio::msgAddEvent(Event& event, Part* part, bool doUndoFlag, bool doCtrls, bool doClones, bool waitRead)/*{{{*/
{
    AudioMsg msg;
    msg.id = SEQM_ADD_EVENT;
    msg.ev1 = event;
    msg.p2 = part;
    msg.a = doCtrls;
    msg.b = doClones;
    sendMessage(&msg, doUndoFlag, waitRead);
}/*}}}*/

void Audio::msgAddEventCheck(MidiTrack* track, Event& e, bool doUndoFlag, bool doCtrls, bool doClones, bool waitRead)/*{{{*/
{
    AudioMsg msg;
    msg.id = SEQM_ADD_EVENT_CHECK;
    msg.track = track;
    msg.ev1 = e;
    msg.a = doCtrls;
    msg.b = doClones;
    sendMessage(&msg, doUndoFlag, waitRead);
}/*}}}*/

//---------------------------------------------------------
//   msgDeleteEvent
//---------------------------------------------------------

//void Audio::msgDeleteEvent(Event& event, Part* part, bool doUndoFlag)

void Audio::msgDeleteEvent(Event& event, Part* part, bool doUndoFlag, bool doCtrls, bool doClones, bool waitRead)
{
    AudioMsg msg;
    msg.id = SEQM_REMOVE_EVENT;
    msg.ev1 = event;
    msg.p2 = part;
    msg.a = doCtrls;
    msg.b = doClones;
    sendMessage(&msg, doUndoFlag, waitRead);
}

//---------------------------------------------------------
//   msgChangeEvent
//---------------------------------------------------------

//void Audio::msgChangeEvent(Event& oe, Event& ne, Part* part, bool doUndoFlag)

void Audio::msgChangeEvent(Event& oe, Event& ne, Part* part, bool doUndoFlag, bool doCtrls, bool doClones, bool waitRead)
{
    AudioMsg msg;
    msg.id = SEQM_CHANGE_EVENT;
    msg.ev1 = oe;
    msg.ev2 = ne;
    msg.p3 = part;
    msg.a = doCtrls;
    msg.b = doClones;
    sendMessage(&msg, doUndoFlag, waitRead);
}

//---------------------------------------------------------
//   msgAddTempo
//---------------------------------------------------------

void Audio::msgAddTempo(int tick, int tempo, bool doUndoFlag)
{
    AudioMsg msg;
    msg.id = SEQM_ADD_TEMPO;
    msg.a = tick;
    msg.b = tempo;
    sendMessage(&msg, doUndoFlag);
}

//---------------------------------------------------------
//   msgSetTempo
//---------------------------------------------------------

void Audio::msgSetTempo(int tick, int tempo, bool doUndoFlag)
{
    AudioMsg msg;
    msg.id = SEQM_SET_TEMPO;
    msg.a = tick;
    msg.b = tempo;
    sendMessage(&msg, doUndoFlag);
}

//---------------------------------------------------------
//   msgSetGlobalTempo
//---------------------------------------------------------

void Audio::msgSetGlobalTempo(int val)
{
    AudioMsg msg;
    msg.id = SEQM_SET_GLOBAL_TEMPO;
    msg.a = val;
    sendMessage(&msg, false);
}

//---------------------------------------------------------
//   msgDeleteTempo
//---------------------------------------------------------

void Audio::msgDeleteTempo(int tick, int tempo, bool doUndoFlag)
{
    AudioMsg msg;
    msg.id = SEQM_REMOVE_TEMPO;
    msg.a = tick;
    msg.b = tempo;
    sendMessage(&msg, doUndoFlag);
}

//---------------------------------------------------------
//   msgDeleteTempoRange
//---------------------------------------------------------

void Audio::msgDeleteTempoRange(QList<void*> tempo, bool doUndoFlag)
{
    AudioMsg msg;
    msg.id = SEQM_REMOVE_TEMPO_RANGE;
    msg.objectList = tempo;
    sendMessage(&msg, doUndoFlag);
}

//---------------------------------------------------------
//   msgAddSig
//---------------------------------------------------------

void Audio::msgAddSig(int tick, int z, int n, bool doUndoFlag)
{
    AudioMsg msg;
    msg.id = SEQM_ADD_SIG;
    msg.a = tick;
    msg.b = z;
    msg.c = n;
    sendMessage(&msg, doUndoFlag);
}

//---------------------------------------------------------
//   msgRemoveSig
//! sends remove tempo signature message
//---------------------------------------------------------

void Audio::msgRemoveSig(int tick, int z, int n, bool doUndoFlag)
{
    AudioMsg msg;
    msg.id = SEQM_REMOVE_SIG;
    msg.a = tick;
    msg.b = z;
    msg.c = n;
    sendMessage(&msg, doUndoFlag);
}

//---------------------------------------------------------
//   msgScanAlsaMidiPorts
//---------------------------------------------------------

void Audio::msgScanAlsaMidiPorts()
{
    AudioMsg msg;
    msg.id = SEQM_SCAN_ALSA_MIDI_PORTS;
    sendMessage(&msg, false);
}

//---------------------------------------------------------
//   msgResetMidiDevices
//---------------------------------------------------------

void Audio::msgResetMidiDevices()
{
    AudioMsg msg;
    msg.id = SEQM_RESET_DEVICES;
    sendMessage(&msg, false);
}

//---------------------------------------------------------
//   msgInitMidiDevices
//---------------------------------------------------------

void Audio::msgInitMidiDevices()
{
    AudioMsg msg;
    msg.id = SEQM_INIT_DEVICES;
    sendMessage(&msg, false);
}

//---------------------------------------------------------
//   panic
//---------------------------------------------------------

void Audio::msgPanic()
{
    AudioMsg msg;
    msg.id = SEQM_PANIC;
    sendMessage(&msg, false);
}

//---------------------------------------------------------
//   localOff
//---------------------------------------------------------

void Audio::msgLocalOff()
{
    AudioMsg msg;
    msg.id = SEQM_MIDI_LOCAL_OFF;
    sendMessage(&msg, false);
}

//---------------------------------------------------------
//   msgUpdateSoloStates
//---------------------------------------------------------

void Audio::msgUpdateSoloStates()
{
    AudioMsg msg;
    msg.id = SEQM_UPDATE_SOLO_STATES;
    sendMsg(&msg);
}

//---------------------------------------------------------
//   msgPlayMidiEvent
//---------------------------------------------------------

void Audio::msgPlayMidiEvent(const MidiPlayEvent* event)
{
    AudioMsg msg;
    msg.id = SEQM_PLAY_MIDI_EVENT;
    msg.p1 = event;
    sendMessage(&msg, false);
}

//---------------------------------------------------------
//   msgSetHwCtrlState
//---------------------------------------------------------

void Audio::msgSetHwCtrlState(MidiPort* port, int ch, int ctrl, int val)
{
    AudioMsg msg;
    msg.id = SEQM_SET_HW_CTRL_STATE;
    msg.p1 = port;
    msg.a = ch;
    msg.b = ctrl;
    msg.c = val;
    sendMessage(&msg, false);
}

//---------------------------------------------------------
//   msgSetHwCtrlState
//---------------------------------------------------------

void Audio::msgSetHwCtrlStates(MidiPort* port, int ch, int ctrl, int val, int lastval)
{
    AudioMsg msg;
    msg.id = SEQM_SET_HW_CTRL_STATE;
    msg.p1 = port;
    msg.a = ch;
    msg.b = ctrl;
    msg.c = val;
    msg.ival = lastval;
    sendMessage(&msg, false);
}

//---------------------------------------------------------
//   msgSetTrackOutChannel
//---------------------------------------------------------

void Audio::msgSetTrackOutChannel(MidiTrack* track, int ch)
{
    AudioMsg msg;
    msg.id = SEQM_SET_TRACK_OUT_CHAN;
    msg.p1 = track;
    msg.a = ch;
    sendMessage(&msg, false);
}

//---------------------------------------------------------
//   msgSetTrackOutPort
//---------------------------------------------------------

void Audio::msgSetTrackOutPort(MidiTrack* track, int port)
{
    AudioMsg msg;
    msg.id = SEQM_SET_TRACK_OUT_PORT;
    msg.p1 = track;
    msg.a = port;
    sendMessage(&msg, false);
}

//---------------------------------------------------------
//   msgIdle
//---------------------------------------------------------

void Audio::msgIdle(bool on)
{
    AudioMsg msg;
    msg.id = SEQM_IDLE;
    msg.a = on;
    sendMessage(&msg, false);
}

void Audio::msgPreloadCtrl()
{
    AudioMsg msg;
    msg.id = SEQM_PRELOAD_PROGRAM;
    sendMsg(&msg);
}

