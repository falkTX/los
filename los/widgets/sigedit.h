//=========================================================
//  OOMidi
//  OpenOctave Midi and Audio Editor
//    $Id: sigedit.h,v 1.1.1.1.2.1 2004/12/28 23:23:51 lunar_shuttle Exp $
//  (C) Copyright 2002 Werner Schweer (ws@seh.de)
//=========================================================

#ifndef __SIGEDIT_H__
#define __SIGEDIT_H__

#include "../sig.h"

#include <QAbstractSpinBox>

//---------------------------------------------------------
//   SigEdit
//---------------------------------------------------------

class SigEdit : public QAbstractSpinBox
{
    Q_OBJECT

    TimeSignature _sig;
    bool initialized;

    virtual void paintEvent(QPaintEvent* event);
    virtual void stepBy(int steps);
    virtual StepEnabled stepEnabled() const;
    virtual void fixup(QString& input) const;
    virtual QValidator::State validate(QString&, int&) const;
    void updateValue();
    int curSegment() const;
    virtual bool event(QEvent*);

signals:
    void valueChanged(const TimeSignature&);
void returnPressed();

public slots:
    void setValue(const TimeSignature&);
    void setValue(const QString& s);

public:
    SigEdit(QWidget* parent = 0);
    ~SigEdit();
    const TimeSignature& sig() { return _sig; }
};

#endif
