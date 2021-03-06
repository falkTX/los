//=========================================================
//  LOS
//  Libre Octave Studio
//    $Id: marker.cpp,v 1.2 2003/12/10 18:34:22 wschweer Exp $
//  (C) Copyright 2000 Werner Schweer (ws@seh.de)
//=========================================================

#include "marker.h"
#include "xml.h"

Marker* MarkerList::add(const Marker& marker)
{
	iMarker i = insert(std::pair<const int, Marker > (marker.tick(), Marker(marker)));
	return &i->second;
}

Marker* MarkerList::add(const QString& s, int t, bool lck)
{
	Marker marker(s);
	marker.setType(lck ? Pos::FRAMES : Pos::TICKS);
	marker.setTick(t);
	iMarker i = insert(std::pair<const int, Marker > (t, marker));
	return &i->second;
}

//---------------------------------------------------------
//   read
//---------------------------------------------------------

void Marker::read(Xml& xml)
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
				xml.unknown("Marker");
				break;
			case Xml::Attribut:
				if (tag == "tick")
					setTick(xml.s2().toInt());
				else if (tag == "lock")
					setType(xml.s2().toInt() ? FRAMES : TICKS);
				else if (tag == "name")
				{
					_name = xml.s2();
				}
				else if(tag == "id")
				{
					m_id = xml.s2().toLongLong();
				}
				break;
			case Xml::TagEnd:
				if (xml.s1() == "marker")
					return;
			default:
				break;
		}
	}
}

//---------------------------------------------------------
//   write
//---------------------------------------------------------

void MarkerList::write(int level, Xml& xml) const
{
	for (ciMarker i = begin(); i != end(); ++i)
	{
		const Marker& m = i->second;
		xml.put(level, "<marker tick=\"%d\" lock=\"%d\" name=\"%s\" id=\"%lld\"/>",
				m.tick(), m.type() == Pos::FRAMES, Xml::xmlString(m.name()).toLatin1().constData(), m.id());
	}
}

//---------------------------------------------------------
//   remove
//---------------------------------------------------------

void MarkerList::remove(Marker* m)
{
	for (iMarker i = begin(); i != end(); ++i)
	{
		Marker* mm = &i->second;
		if (mm == m)
		{
			erase(i);
			return;
		}
	}
	printf("MarkerList::remove(): marker not found\n");
}

