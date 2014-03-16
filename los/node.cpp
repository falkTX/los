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

#include "node.h"
#include "globals.h"
#include "gconfig.h"
#include "song.h"
#include "xml.h"
#include "audiodev.h"
#include "audio.h"
#include "utils.h"      //debug
#include "al/dsp.h"
#include "midictrl.h"
#include "mididev.h"
#include "midiport.h"
#include "midimonitor.h"

// Uncomment this (and make sure to set Jack buffer size high like 2048)
//  to see process flow messages.
//#define NODE_DEBUG
//#define FIFO_DEBUG

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
    if (n > MAX_CHANNELS)
        _channels = MAX_CHANNELS;
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

//---------------------------------------------------------
//   Fifo
//---------------------------------------------------------

Fifo::Fifo()
{
    los_atomic_init(&count);
    //nbuffer = FIFO_BUFFER;
    nbuffer = fifoLength;
    buffer = new FifoBuffer*[nbuffer];
    for (int i = 0; i < nbuffer; ++i)
        buffer[i] = new FifoBuffer;
    clear();
}

Fifo::~Fifo()
{
    for (int i = 0; i < nbuffer; ++i)
    {
        // p3.3.45
        if (buffer[i]->buffer)
        {
            //printf("Fifo::~Fifo freeing buffer\n");
            free(buffer[i]->buffer);
        }

        delete buffer[i];
    }

    delete[] buffer;
    los_atomic_destroy(&count);
}

//---------------------------------------------------------
//   put
//    return true if fifo full
//---------------------------------------------------------

bool Fifo::put(int segs, unsigned long samples, float** src, unsigned pos)
{
#ifdef FIFO_DEBUG
    printf("FIFO::put segs:%d samples:%lu pos:%u\n", segs, samples, pos);
#endif

    if (los_atomic_read(&count) == nbuffer)
    {
        if(debugMsg)
            printf("FIFO %p overrun... %d\n", this, count.counter);
        return true;
    }
    FifoBuffer* b = buffer[widx];
    int n = segs * samples;
    if (b->maxSize < n)
    {
        if (b->buffer)
        {
            //delete[] b->buffer;
            free(b->buffer);
            b->buffer = 0;
        }
        posix_memalign((void**) &(b->buffer), 16, sizeof (float) * n);
        if (!b->buffer)
        {
            printf("Fifo::put could not allocate buffer segs:%d samples:%lu pos:%u\n", segs, samples, pos);
            return true;
        }

        b->maxSize = n;
    }
    // p3.3.45
    if (!b->buffer)
    {
        printf("Fifo::put no buffer! segs:%d samples:%lu pos:%u\n", segs, samples, pos);
        return true;
    }

    b->size = samples;
    b->segs = segs;
    b->pos = pos;
    for (int i = 0; i < segs; ++i)
        //memcpy(b->buffer + i * samples, src[i], samples * sizeof(float));
        AL::dsp->cpy(b->buffer + i * samples, src[i], samples);
    add();
    return false;
}

//---------------------------------------------------------
//   get
//    return true if fifo empty
//---------------------------------------------------------

bool Fifo::get(int segs, unsigned long samples, float** dst, unsigned* pos)
{
#ifdef FIFO_DEBUG
    printf("FIFO::get segs:%d samples:%lu\n", segs, samples);
#endif

    if (los_atomic_read(&count) == 0)
    {
        if(debugMsg)
            printf("FIFO %p underrun... %d\n", this, count.counter);
        return true;
    }
    FifoBuffer* b = buffer[ridx];
    if (!b->buffer)
    {
        if(debugMsg)
            printf("Fifo::get no buffer! segs:%d samples:%lu b->pos:%u\n", segs, samples, b->pos);
        return true;
    }

    if (pos)
        *pos = b->pos;

    for (int i = 0; i < segs; ++i)
        dst[i] = b->buffer + samples * (i % b->segs);
    remove();
    return false;
}

int Fifo::getCount()
{
    return los_atomic_read(&count);
}
//---------------------------------------------------------
//   remove
//---------------------------------------------------------

void Fifo::remove()
{
    ridx = (ridx + 1) % nbuffer;
    los_atomic_dec(&count);
}

//---------------------------------------------------------
//   getWriteBuffer
//---------------------------------------------------------

bool Fifo::getWriteBuffer(int segs, unsigned long samples, float** buf, unsigned pos)
{
#ifdef FIFO_DEBUG
    printf("Fifo::getWriteBuffer segs:%d samples:%lu pos:%u\n", segs, samples, pos);
#endif

    if (los_atomic_read(&count) == nbuffer)
        return true;
    FifoBuffer* b = buffer[widx];
    int n = segs * samples;
    if (b->maxSize < n)
    {
        if (b->buffer)
        {
            free(b->buffer);
            b->buffer = 0;
        }

        posix_memalign((void**) &(b->buffer), 16, sizeof (float) * n);
        if (!b->buffer)
        {
            printf("Fifo::getWriteBuffer could not allocate buffer segs:%d samples:%lu pos:%u\n", segs, samples, pos);
            return true;
        }

        b->maxSize = n;
    }

    // p3.3.45
    if (!b->buffer)
    {
        printf("Fifo::getWriteBuffer no buffer! segs:%d samples:%lu pos:%u\n", segs, samples, pos);
        return true;
    }

    for (int i = 0; i < segs; ++i)
        buf[i] = b->buffer + i * samples;

    b->size = samples;
    b->segs = segs;
    b->pos = pos;
    return false;
}

//---------------------------------------------------------
//   add
//---------------------------------------------------------

void Fifo::add()
{
    widx = (widx + 1) % nbuffer;
    los_atomic_inc(&count);
}
