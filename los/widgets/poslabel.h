//=========================================================
//  LOS
//  Libre Octave Studio
//    $Id: poslabel.h,v 1.2 2004/01/11 18:55:37 wschweer Exp $
//  (C) Copyright 2001 Werner Schweer (ws@seh.de)
//=========================================================

#ifndef __POSLABEL_H__
#define __POSLABEL_H__

#include <QLabel>

//---------------------------------------------------------
//   PosLabel
//---------------------------------------------------------

// FIXME - remove setTickValue for setValue only

class PosLabel : public QLabel
{
    Q_OBJECT

    unsigned _tickValue;
    //unsigned _sampleValue;

    void updateValue();

protected:
    QSize sizeHint() const;

public slots:
    void setTickValue(unsigned);
    //void setSampleValue(unsigned);
    void setValue(unsigned);

public:
    PosLabel(QWidget* parent, const char* name = 0);

    unsigned value() const
    {
        return _tickValue;
    }

    unsigned tickValue() const
    {
        return _tickValue;
    }

//    unsigned sampleValue() const
//    {
//        return _sampleValue;
//    }
};

#endif
