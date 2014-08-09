//=========================================================
//  LOS
//  Libre Octave Studio
//  $Id: node.cpp,v 1.36.2.25 2009/12/20 05:00:35 terminator356 Exp $
//
//  (C) Copyright 2000-2004 Werner Schweer (ws@seh.de)
//=========================================================

#include <cmath>
#include <assert.h>
#include <stdlib.h>

#include "globals.h"
#include "gconfig.h"
#include "song.h"
#include "xml.h"
#include "audiodev.h"
#include "audio.h"
#include "utils.h"      //debug
#include "midictrl.h"
#include "mididev.h"
#include "midiport.h"
#include "midimonitor.h"

//---------------------------------------------------------
//   isMute
//---------------------------------------------------------

bool MidiTrack::isMute() const
{
    if (_solo || (_internalSolo && !_mute))
        return false;

    if (_soloRefCnt)
        return true;

    return _mute;
}

//---------------------------------------------------------
//   setSolo
//---------------------------------------------------------

void MidiTrack::setSolo(bool val, bool monitor)
{
    if (_solo != val)
    {
        _solo = val;
        updateSoloStates(false);
    }
    if(!monitor)
    {
        //Call the monitor here if it was not called from the monitor
        midiMonitor->msgSendMidiOutputEvent(this, CTRL_SOLO, val ? 127 : 0);
    }
}

//---------------------------------------------------------
//   setInternalSolo
//---------------------------------------------------------

void Track::setInternalSolo(unsigned int val)
{
        _internalSolo = val;
}

//---------------------------------------------------------
//   clearSoloRefCounts
//   This is a static member function. Required for outside access.
//   Clears the internal static reference counts.
//---------------------------------------------------------

void Track::clearSoloRefCounts()
{
    _soloRefCnt = 0;
}

//---------------------------------------------------------
//   updateSoloState
//---------------------------------------------------------

void Track::updateSoloState()
{
    if (_solo)
        _soloRefCnt++;
    else
        if (_soloRefCnt && !_tmpSoloChainNoDec)
        _soloRefCnt--;
}

//---------------------------------------------------------
//   updateInternalSoloStates
//---------------------------------------------------------

void Track::updateInternalSoloStates()
{
    if (_tmpSoloChainTrack->solo())
    {
        _internalSolo++;
        _soloRefCnt++;
    }
    else
        if (!_tmpSoloChainNoDec)
    {
        if (_internalSolo)
            _internalSolo--;
        if (_soloRefCnt)
            _soloRefCnt--;
    }
}

//---------------------------------------------------------
//   updateInternalSoloStates
//---------------------------------------------------------

void MidiTrack::updateInternalSoloStates()
{
    if (this == _tmpSoloChainTrack)
        return;

    Track::updateInternalSoloStates();
}

//---------------------------------------------------------
//   updateSoloStates
//---------------------------------------------------------

void MidiTrack::updateSoloStates(bool noDec)
{
    //if (noDec && !_solo)
    if (noDec && !_solo)
        return;

        _tmpSoloChainTrack = this;
        _tmpSoloChainDoIns = false;
        _tmpSoloChainNoDec = noDec;
    updateSoloState();

#if 0 // not required anymore since we create full synth audio ports
        if (outPort() >= 0)
        {
                //MidiDevice *md = midiPorts[outPort()].device();
                //if (md && md->isSynthPlugin())
                        //((SynthPluginDevice*) md)->updateInternalSoloStates();
        }
#endif
}

//---------------------------------------------------------
//   setMute
//---------------------------------------------------------

void Track::setMute(bool val, bool monitor)
{
    _mute = val;
    if(!monitor)
    {//call the monitor with the update if it was not called from the monitor
        //Call the monitor here if it was not called from the monitor
        midiMonitor->msgSendMidiOutputEvent((MidiTrack*)this, CTRL_MUTE, val ? 127 : 0);
    }
}

//---------------------------------------------------------
//   setOff
//---------------------------------------------------------

void Track::setOff(bool val)
{
    _off = val;
}

//---------------------------------------------------------
//   setChannels
//---------------------------------------------------------

void Track::setChannels(int n)
{
    if (n > kMaxAudioChannels)
        _channels = kMaxAudioChannels;
    else
        _channels = n;
    for (int i = 0; i < _channels; ++i)
    {
        _meter[i] = 0.0;
        _peak[i] = 0.0;
    }
}

//---------------------------------------------------------
//   resetMeter
//---------------------------------------------------------

void Track::resetMeter()
{
    for (int i = 0; i < _channels; ++i)
        _meter[i] = 0.0;
}

//---------------------------------------------------------
//   resetPeaks
//---------------------------------------------------------

void Track::resetPeaks()
{
    for (int i = 0; i < _channels; ++i)
        _peak[i] = 0.0;
    _lastActivity = 0;
}

//---------------------------------------------------------
//   resetAllMeter
//---------------------------------------------------------

void Track::resetAllMeter()
{
    MidiTrackList* tl = song->tracks();
    for (iMidiTrack i = tl->begin(); i != tl->end(); ++i)
        (*i)->resetMeter();
}
