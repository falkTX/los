//=========================================================
//  LOS
//  Libre Octave Studio
//  $Id: eventlist.cpp,v 1.7.2.3 2009/11/05 03:14:35 terminator356 Exp $
//
//  (C) Copyright 2000-2003 Werner Schweer (ws@seh.de)
//=========================================================

#include "tempo.h"
#include "event.h"
#include "xml.h"

//---------------------------------------------------------
//   readEventList
//---------------------------------------------------------

void EventList::read(Xml& xml, const char* name)
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
                if (tag == "event")
                {
                    Event e(Note);
                    e.read(xml);
                    add(e);
                }
                else
                    xml.unknown("readEventList");
                break;
            case Xml::TagEnd:
                if (tag == name)
                    return;
            default:
                break;
        }
    }
}

//---------------------------------------------------------
//   add
//---------------------------------------------------------

iEvent EventList::add(Event& event)
{
    return std::multimap<unsigned, Event, std::less<unsigned> >::insert(std::pair<const unsigned, Event > (event.tick(), event));
}

//---------------------------------------------------------
//   move
//---------------------------------------------------------

void EventList::move(Event& event, unsigned tick)
{
    iEvent i = find(event);
    erase(i);

    std::multimap<unsigned, Event, std::less<unsigned> >::insert(std::pair<const unsigned, Event > (tick, event));
}

//---------------------------------------------------------
//   find
//---------------------------------------------------------

iEvent EventList::find(const Event& event)
{
    EventRange range = equal_range(event.tick());

    for (iEvent i = range.first; i != range.second; ++i)
    {
        if (i->second == event)
            return i;
    }
    return end();
}

//---------------------------------------------------------
//   dump
//---------------------------------------------------------

void EventList::dump() const
{
    for (ciEvent i = begin(); i != end(); ++i)
        i->second.dump();
}

