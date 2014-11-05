//=========================================================
//  OOMidi
//  OpenOctave Midi and Audio Editor
//    $Id: posedit.h,v 1.1.1.1.2.1 2004/12/27 19:47:25 lunar_shuttle Exp $
//  (C) Copyright 2001 Werner Schweer (ws@seh.de)
//=========================================================

#ifndef __POSEDIT_H__
#define __POSEDIT_H__

#include "../pos.h"

#include <QAbstractSpinBox>

//---------------------------------------------------------
//   PosEdit
//---------------------------------------------------------

class PosEdit : public QAbstractSpinBox
{
    Q_OBJECT

    Pos _pos;
    bool initialized;

    QIntValidator* validator;

    virtual void paintEvent(QPaintEvent* event);
    virtual void stepBy(int steps);
    virtual StepEnabled stepEnabled() const;
    virtual void fixup(QString& input) const;
    virtual QValidator::State validate(QString&, int&) const;
    void updateValue();
    int curSegment() const;
    virtual bool event(QEvent*);
    void finishEdit();

signals:
    void valueChanged(const Pos&);

    // Choose these three carefully, watch out for focusing recursion.
    void returnPressed();
    void lostFocus();
    // This is emitted when focus lost or return pressed (same as QAbstractSpinBox).
    void editingFinished();

public slots:
    void setValue(const Pos& time);
    void setValue(int t);
    void setValue(const QString& s);

public:
    PosEdit(QWidget* parent = 0);
    ~PosEdit();
    QSize sizeHint() const;

    Pos pos() const
    {
        return _pos;
    }
};

#endif
