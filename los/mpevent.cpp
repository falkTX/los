//=========================================================
//  LOS
//  Libre Octave Studio
//  $Id: mpevent.cpp,v 1.6.2.2 2009/11/25 09:09:43 terminator356 Exp $
//
//  (C) Copyright 2002-2004 Werner Schweer (ws@seh.de)
//=========================================================

#include "mpevent.h"

#include "helper.h"
#include "event.h"
#include "midictrl.h"
#include "midiport.h"
#include "los/midi.h"

//---------------------------------------------------------
//   MEvent
//---------------------------------------------------------

MEvent::MEvent(unsigned t, int port, int tpe, const unsigned char* data, int len, MidiTrack* trk)
{
    _time = t;
    _port = port;
    edata.setData(data, len);
    _type = tpe;
    _loopNum = 0;
    m_track = trk;
    m_source = SystemSource;
}

MEvent::MEvent(unsigned tick, int port, int channel, const Event& e, MidiTrack* trk)
{
    m_track = trk;
    m_source = SystemSource;
    setChannel(channel);
    setTime(tick);
    setPort(port);
    setLoopNum(0);
    switch (e.type())
    {
        case Note:
            setType(ME_NOTEON);
            setA(e.dataA());
            setB(e.dataB());
            break;
        case Controller:
            setType(ME_CONTROLLER);
            setA(e.dataA()); // controller number
            setB(e.dataB()); // controller value
            break;
        case PAfter:
            setType(ME_POLYAFTER);
            setA(e.dataA());
            setB(e.dataB());
            break;
        case CAfter:
            setType(ME_AFTERTOUCH);
            setA(e.dataA());
            setB(0);
            break;
        case Sysex:
            setType(ME_SYSEX);
            setData(e.eventData());
            break;
        default:
            printf("MEvent::MEvent(): event type %d not implemented\n",
                    type());
            break;
    }
}

//---------------------------------------------------------
//   dump
//---------------------------------------------------------

void MEvent::dump() const
{
    printf("time:%d port:%d chan:%d ", _time, _port, _channel + 1);
    if (_type == 0x90)
    { // NoteOn
        QString s = pitch2string(_a);
        printf("NoteOn %s(0x%x) %d\n", s.toLatin1().constData(), _a, _b);
    }
    else if (_type == 0xf0)
    {
        printf("SysEx len %d 0x%0x ...\n", len(), data()[0]);
    }
    else
        printf("type:0x%02x a=%d b=%d\n", _type, _a, _b);
}

//---------------------------------------------------------
//   operator <
//---------------------------------------------------------

bool MEvent::operator<(const MEvent& e) const
{
    if (time() != e.time())
        return time() < e.time();
    if (port() != e.port())
        return port() < e.port();

    // play note off events first to prevent overlapping
    // notes

    if (channel() == e.channel())
        return type() == ME_NOTEOFF
            || (type() == ME_NOTEON && dataB() == 0);

    int map[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 10, 11, 12, 13, 14, 15};
    return map[channel()] < map[e.channel()];
}


//---------------------------------------------------------
//   put
//    return true on fifo overflow
//---------------------------------------------------------

bool MidiFifo::put(const MidiPlayEvent& event)
{
    if (size < MIDI_FIFO_SIZE)
    {
        fifo[wIndex] = event;
        wIndex = (wIndex + 1) % MIDI_FIFO_SIZE;
        // q_atomic_increment(&size);
        ++size;
        return false;
    }
    return true;
}

//---------------------------------------------------------
//   get
//---------------------------------------------------------

MidiPlayEvent MidiFifo::get()
{
    MidiPlayEvent event(fifo[rIndex]);
    rIndex = (rIndex + 1) % MIDI_FIFO_SIZE;
    // q_atomic_decrement(&size);
    --size;
    return event;
}

//---------------------------------------------------------
//   peek
//---------------------------------------------------------

const MidiPlayEvent& MidiFifo::peek(int n)
{
    int idx = (rIndex + n) % MIDI_FIFO_SIZE;
    return fifo[idx];
}

//---------------------------------------------------------
//   remove
//---------------------------------------------------------

void MidiFifo::remove()
{
    rIndex = (rIndex + 1) % MIDI_FIFO_SIZE;
    --size;
}

//---------------------------------------------------------
//   put
//    return true on fifo overflow
//---------------------------------------------------------

bool MidiRecFifo::put(const MidiPlayEvent& event)
{
    if (size < MIDI_REC_FIFO_SIZE)
    {
        fifo[wIndex] = event;
        wIndex = (wIndex + 1) % MIDI_REC_FIFO_SIZE;
        ++size;
        return false;
    }
    return true;
}

//---------------------------------------------------------
//   get
//---------------------------------------------------------

MidiPlayEvent MidiRecFifo::get()
{
    MidiPlayEvent event(fifo[rIndex]);
    rIndex = (rIndex + 1) % MIDI_REC_FIFO_SIZE;
    --size;
    return event;
}

//---------------------------------------------------------
//   peek
//---------------------------------------------------------

const MidiPlayEvent& MidiRecFifo::peek(int n)
{
    int idx = (rIndex + n) % MIDI_REC_FIFO_SIZE;
    return fifo[idx];
}

//---------------------------------------------------------
//   remove
//---------------------------------------------------------

void MidiRecFifo::remove()
{
    rIndex = (rIndex + 1) % MIDI_REC_FIFO_SIZE;
    --size;
}
