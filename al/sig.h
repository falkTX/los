//=============================================================================
//  AL
//  Audio Utility Library
//  $Id:$
//
//  Copyright (C) 2002-2006 by Werner Schweer and others
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License version 2.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//=============================================================================

#ifndef __SIG_H__
#define __SIG_H__

#include <map>

#include "al.h"

class Xml;

namespace AL
{

static const uint kMaxTick = 0x7fffffff/100;

//---------------------------------------------------------
//   TimeSignature
//---------------------------------------------------------

struct TimeSignature {
    int z, n;

    TimeSignature() noexcept
        : z(4), n(4) {}

    TimeSignature(const int z2, const int n2) noexcept
        : z(z2), n(n2) {}

    TimeSignature(const TimeSignature& ts) noexcept
        : z(ts.z), n(ts.n) {}

    bool isValid() const noexcept
    {
        if (z < 1 || z > 63)
            return false;

        switch (n)
        {
        case 1:
        case 2:
        case 3:
        case 4:
        case 8:
        case 16:
        case 32:
        case 64:
        case 128:
            return true;
        }

        return false;
    }

    TimeSignature& operator=(const TimeSignature& ts) noexcept
    {
        z = ts.z;
        n = ts.n;
        return *this;
    }
};

//---------------------------------------------------------
//   Signature Event
//---------------------------------------------------------

struct SigEvent {
    TimeSignature sig;
    uint tick; // signature valid from this position
    int  bar;  // precomputed

    SigEvent() noexcept
        : sig(),
          tick(0),
          bar(0) {}

    SigEvent(const TimeSignature& sig2, uint tick2) noexcept
        : sig(sig2),
          tick(tick2),
          bar(0) {}

    SigEvent(const SigEvent& sig2) noexcept
        : sig(sig2.sig),
          tick(sig2.tick),
          bar(sig2.bar) {}

    SigEvent& operator=(const SigEvent& sig2) noexcept
    {
        sig  = sig2.sig;
        tick = sig2.tick;
        bar  = sig2.bar;
        return *this;
    }

    int read(Xml&);
    void write(int, Xml&, int) const;
};

//---------------------------------------------------------
//   SigList
//---------------------------------------------------------

typedef std::map<unsigned, SigEvent*, std::less<unsigned> > SIGLIST;
typedef SIGLIST::iterator iSigEvent;
typedef SIGLIST::const_iterator ciSigEvent;
typedef SIGLIST::reverse_iterator riSigEvent;
typedef SIGLIST::const_reverse_iterator criSigEvent;

class SigList : public SIGLIST
{
    int ticks_beat(int N) const;
    void normalize();
    int ticksMeasure(const TimeSignature&) const;
    int ticksMeasure(int z, int n) const;

public:
    SigList();
    void clear();
    void add(unsigned tick, const TimeSignature& s);
    //void add(unsigned tick, int z, int n);
    void del(unsigned tick);

    ///void read(QDomNode);
    ///void write(Xml&) const;
    void read(Xml&);
    void write(int, Xml&) const;

    void dump() const;

    TimeSignature timesig(unsigned tick) const;
    void timesig(unsigned tick, int& z, int& n) const;
    void tickValues(unsigned t, int* bar, int* beat, unsigned* tick) const;
    unsigned bar2tick(int bar, int beat, unsigned tick) const;

    int ticksMeasure(unsigned tick) const;
    int ticksBeat(unsigned tick) const;
    unsigned raster(unsigned tick, int raster) const;
    unsigned raster1(unsigned tick, int raster) const; // round down
    unsigned raster2(unsigned tick, int raster) const; // round up
    int rasterStep(unsigned tick, int raster) const;
};

extern SigList sigmap;

} // namespace AL

#endif
