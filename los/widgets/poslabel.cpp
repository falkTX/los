//=========================================================
//  LOS
//  Libre Octave Studio
//    $Id: poslabel.cpp,v 1.2.2.2 2009/04/06 01:24:55 terminator356 Exp $
//  (C) Copyright 2001 Werner Schweer (ws@seh.de)
//=========================================================

#include <stdlib.h>
#include <cmath>

#include <QApplication>
#include <QStyle>

#include "poslabel.h"

#include "sig.h"
#include "tempo.h"
#include "globals.h"

//---------------------------------------------------------
//   PosLabel
//---------------------------------------------------------

PosLabel::PosLabel(QWidget* parent, const char* name)
: QLabel(parent)
{
    setObjectName(name);
    _tickValue = 0;
    //_sampleValue = 0;
    setFrameStyle(WinPanel | Sunken);
    setLineWidth(2);
    setMidLineWidth(3);
    //int fw = style()->pixelMetric(QStyle::PM_DefaultFrameWidth, 0, this); // ddskrjo 0
    int fw = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    setIndent(fw);
    updateValue();
}

//---------------------------------------------------------
//   sizeHint
//---------------------------------------------------------

QSize PosLabel::sizeHint() const
{
    QFontMetrics fm(font());
    //int fw = style()->pixelMetric(QStyle::PM_DefaultFrameWidth, 0, this); // ddskrjo 0
    int fw = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    int h = fm.height() + fw * 2;
    int w = 2 + fm.width('9') * 9 + fm.width('.') * 2 + fw * 4;
    return QSize(w, h).expandedTo(QApplication::globalStrut());
}

//---------------------------------------------------------
//   updateValue
//---------------------------------------------------------

void PosLabel::updateValue()
{
    QString s;
    int bar, beat;
    unsigned tick;
    sigmap.tickValues(_tickValue, &bar, &beat, &tick);
    s.sprintf("%04d.%02d.%03u", bar + 1, beat + 1, tick);
    setText(s);
}

//---------------------------------------------------------
//   setTickValue
//---------------------------------------------------------

void PosLabel::setTickValue(unsigned val)
{
    if (val == _tickValue)
        return;
    if (val >= MAX_TICK)
        abort();
    _tickValue = val;
    updateValue();
}

//---------------------------------------------------------
//   setValue
//---------------------------------------------------------

// should probably be replaced by setTickValue

void PosLabel::setValue(unsigned val)
{
    unsigned oval = _tickValue;
    if (val == oval)
        return;
    _tickValue = val;
    updateValue();
}
