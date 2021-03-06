//=========================================================
//  LOS
//  Libre Octave Studio
//  $Id: event.h,v 1.7.2.4 2009/12/20 05:00:35 terminator356 Exp $
//
//  (C) Copyright 1999-2004 Werner Schweer (ws@seh.de)
//=========================================================

#ifndef __EVENT_H__
#define __EVENT_H__

#include <map>
#include <sys/types.h>
#include <QString>

#include "pos.h"
#include "evdata.h"

enum EventType
{
    Note, Controller, Sysex, PAfter, CAfter, Meta
};

class Xml;
class EventBase;

//---------------------------------------------------------
//   Event
//---------------------------------------------------------

class Event
{
    EventBase* ev;

public:
    Event();
    Event(EventType t);
    Event(const Event& e);
    Event(EventBase* eb);

    virtual ~Event();

    bool empty() const;
    EventType type() const;

    void setType(EventType t);
    Event & operator=(const Event& e);
    bool operator==(const Event& e) const;

    int getRefCount() const;
    bool selected() const;
    void setSelected(bool val);
    void move(int offset);

    void read(Xml& xml);
    //void write(int a, Xml& xml, const Pos& offset) const;
    void write(int a, Xml& xml, const Pos& offset, bool ForceWavePaths = false) const;
    void dump(int n = 0) const;
    Event clone();
    Event mid(unsigned a, unsigned b);

    bool isNote() const;
    bool isNoteOff() const;
    bool isNoteOff(const Event& e) const;
    int dataA() const;
    int pitch() const;
    void setA(int val);
    void setPitch(int val);
    int dataB() const;
    int velo() const;
    void setB(int val);
    void setVelo(int val);
    int dataC() const;
    int veloOff() const;
    void setC(int val);
    void setVeloOff(int val);

    const unsigned char* data() const;
    int dataLen() const;
    void setData(const unsigned char* data, int len);
    const EvData eventData() const;

    const QString name() const;
    void setName(const QString& s);
    int spos() const;
    void setSpos(int s);
    int rightClip() const;
    void setRightClip(int clip);
    int leftClip() const;
    void setLeftClip(int clip);

    void setTick(unsigned val);
    unsigned tick() const;
    unsigned frame() const;
    void setFrame(unsigned val);
    void setLenTick(unsigned val);
    void setLenFrame(unsigned val);
    unsigned lenTick() const;
    unsigned lenFrame() const;
    Pos end() const;
    unsigned endTick() const;
    unsigned endFrame() const;
    void setPos(const Pos& p);
};

typedef std::multimap <unsigned, Event, std::less<unsigned> > EL;
typedef EL::iterator iEvent;
typedef EL::reverse_iterator riEvent;
typedef EL::const_iterator ciEvent;
typedef std::pair <iEvent, iEvent> EventRange;

//---------------------------------------------------------
//   EventList
//    tick sorted list of events
//---------------------------------------------------------

class EventList : public EL
{
    int ref; // number of references to this EventList
    int aref; // number of active references (exclude undo list)
    void deselect();

public:

    EventList()
    {
        ref = 0;
        aref = 0;
    }

    ~EventList()
    {
    }

    void incRef(int n)
    {
        ref += n;
    }

    int refCount() const
    {
        return ref;
    }

    void incARef(int n)
    {
        aref += n;
    }

    int arefCount() const
    {
        return aref;
    }

    iEvent find(const Event&);
    iEvent add(Event& event);
    void move(Event& event, unsigned tick);
    void dump() const;
    void read(Xml& xml, const char* name);
};

#endif

