//=========================================================
//  LOS
//  Libre Octave Studio
//  $Id: midiseq.h,v 1.6.2.11 2009/12/20 05:00:35 terminator356 Exp $
//
//  (C) Copyright 2003 Werner Schweer (ws@seh.de)
//=========================================================

#ifndef __MIDISEQ_H__
#define __MIDISEQ_H__

#include "thread.h"
#include "mpevent.h"
#include "driver/alsatimer.h"
#include "driver/rtctimer.h"

class MidiPort;
class MidiDevice;
class MPEventList;

//---------------------------------------------------------
//   MidiSeq
//---------------------------------------------------------

class MidiSeq : public Thread
{
    int realRtcTicks;
    int timerFd;
    int idle;
    int prio; // realtime priority
    static int ticker;

    /* Testing */
    bool playStateExt; // used for keeping play state in sync functions
    int recTick; // ext sync tick position
    //      int lastTickPos;        // position of last sync tick
    // run values:
    //      unsigned _midiTick;
    double mclock1, mclock2;
    double songtick1, songtick2;
    int recTick1, recTick2;
    int lastTempo;
    double timediff[24];
    int storedtimediffs;

    /* Testing */
    Timer *timer;

    signed int selectTimer();
    bool setRtcTicks();
    static void midiTick(void* p, void*);
    void processTimerTick();
    void processSeek();
    void processStop();
    virtual void processMsg(const ThreadMsg*);
    void updatePollFd();

public:
    //MidiSeq(int prio, const char* name);
    MidiSeq(const char* name);

    ~MidiSeq();

    //bool start();
    virtual void start(int);

    virtual void threadStop();
    virtual void threadStart(void*);

    void msgMsg(int id);
    void msgProcess();
    void msgPreloadCtrl();
    void msgSeek();
    void msgStop();
    void msgSetRtc();
    void msgUpdatePollFd();
    void msgSetMidiDevice(MidiPort*, MidiDevice*);
};

extern MidiSeq* midiSeq;
extern volatile bool midiBusy;
#endif

