//=========================================================
//  OOMidi
//  OpenOctave Midi and Audio Editor
//    $Id: posedit.cpp,v 1.3.2.2 2008/05/21 00:28:54 terminator356 Exp $
//  (C) Copyright 2001 Werner Schweer (ws@seh.de)
//=========================================================

#include "posedit.h"
#include "../sig.h"

#include <QApplication>
#include <QKeyEvent>
#include <QLineEdit>
#include <QStyle>

#include <cstdio>

//---------------------------------------------------------
//   PosEdit
//---------------------------------------------------------

PosEdit::PosEdit(QWidget* parent)
    : QAbstractSpinBox(parent)
{
    validator = new QIntValidator(this);

    initialized = false;
    setReadOnly(false);

    //connect(this, SIGNAL(editingFinished()), SLOT(finishEdit()));
    //connect(this, SIGNAL(returnPressed()), SLOT(enterPressed()));
}

PosEdit::~PosEdit()
{
}

QSize PosEdit::sizeHint() const
{
    QFontMetrics fm(font());
    int fw = style()->pixelMetric(QStyle::PM_SpinBoxFrameWidth);
    int h = fm.height() + fw * 2;
    int w = fw * 4 + 10; // HACK: 10 = spinbox up/down arrows
    w += 2 + fm.width('9') * 9 + fm.width('.') * 2 + fw * 4;
    return QSize(w, h).expandedTo(QApplication::globalStrut());
}

//---------------------------------------------------------
//   event
//    filter Tab and Backtab key events
//---------------------------------------------------------

bool PosEdit::event(QEvent* event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent* ke = static_cast<QKeyEvent*> (event);
        if (ke->key() == Qt::Key_Return)
        {
            //printf("key press event Return\n");   // REMOVE Tim.
            //enterPressed();
            finishEdit();
            emit returnPressed();
            emit editingFinished();
            return true;
        }

        if (ke->key() == Qt::Key_Escape)
        {
            //printf("key press event Escape\n");   // REMOVE Tim.
            if (lineEdit())
                lineEdit()->undo();
            // "By default, isAccepted() is set to true, but don't rely on this as subclasses may
            //   choose to clear it in their constructor."
            // Just to be sure. Otherwise escape will close a midi editor for example, which is annoying.
            ke->setAccepted(true);
            return true;
        }

        int segment = curSegment();
        if (ke->key() == Qt::Key_Backtab)
        {
            if (segment == 2)
            {
                lineEdit()->setSelection(5, 2);
                return true;
            }
            if (segment == 1)
            {
                lineEdit()->setSelection(0, 4);
                return true;
            }
        }
        if (ke->key() == Qt::Key_Tab)
        {
            if (segment == 0)
            {
                lineEdit()->setSelection(5, 2);
                return true;
            }
            if (segment == 1)
            {
                lineEdit()->setSelection(8, 3);
                return true;
            }
        }
    }
    else if (event->type() == QEvent::FocusIn)
    {
        QFocusEvent* fe = static_cast<QFocusEvent*> (event);
        QAbstractSpinBox::focusInEvent(fe);
        int segment = curSegment();
        switch (segment)
        {
        case 0: lineEdit()->setSelection(0, 4);
            break;
        case 1: lineEdit()->setSelection(5, 2);
            break;
        case 2: lineEdit()->setSelection(8, 3);
            break;
        }
        return true;
    }
    else if (event->type() == QEvent::FocusOut)
    {
        QFocusEvent* fe = static_cast<QFocusEvent*> (event);
        QAbstractSpinBox::focusOutEvent(fe);
        finishEdit();
        emit lostFocus();
        emit editingFinished();
        return true;
    }

    return QAbstractSpinBox::event(event);
}


//---------------------------------------------------------
//   setValue
//---------------------------------------------------------

void PosEdit::setValue(const Pos& time)
{
    if (_pos == time)
        return;
    _pos = time;
    updateValue();
}

void PosEdit::setValue(const QString& s)
{
    Pos time(s);
    setValue(time);
}

void PosEdit::setValue(int t)
{
    Pos time(t);
    setValue(time);
}

//---------------------------------------------------------
//   updateValue
//---------------------------------------------------------

void PosEdit::updateValue()
{
    char buffer[64];
    int bar, beat;
    int tick;
    _pos.mbt(&bar, &beat, &tick);
    sprintf(buffer, "%04d.%02d.%03d", bar + 1, beat + 1, tick);
    lineEdit()->setText(buffer);
}

//---------------------------------------------------------
//   stepEnables
//---------------------------------------------------------

QAbstractSpinBox::StepEnabled PosEdit::stepEnabled() const
{
    int segment = curSegment();
    QAbstractSpinBox::StepEnabled en = QAbstractSpinBox::StepUpEnabled | QAbstractSpinBox::StepDownEnabled;

    int bar, beat;
    unsigned tick;
    sigmap.tickValues(_pos.tick(), &bar, &beat, &tick);
    unsigned tb = sigmap.ticksBeat(_pos.tick());
    unsigned tm = sigmap.ticksMeasure(_pos.tick());
    int bm = tm / tb;

    switch (segment)
    {
    case 0:
        if (bar == 0)
            en &= ~QAbstractSpinBox::StepDownEnabled;
        break;
    case 1:
        if (beat == 0)
            en &= ~QAbstractSpinBox::StepDownEnabled;
        else
        {
            if (beat >= (bm - 1))
                en &= ~QAbstractSpinBox::StepUpEnabled;
        }
        break;
    case 2:
        if (tick == 0)
            en &= ~QAbstractSpinBox::StepDownEnabled;
        else
        {
            if (tick >= (tb - 1))
                en &= ~QAbstractSpinBox::StepUpEnabled;
        }
        break;
    }
    return en;
}

//---------------------------------------------------------
//   fixup
//---------------------------------------------------------

void PosEdit::fixup(QString& input) const
{
    printf("fixup <%s>\n", input.toLatin1().constData()); // REMOVE Tim.
}

//---------------------------------------------------------
//   validate
//---------------------------------------------------------

QValidator::State PosEdit::validate(QString& s, int& /*i*/) const
{
    //printf("validate string:%s int:%d\n", s.toLatin1().data(), i);  // REMOVE Tim.
    //printf("validate string:%s\n", s.toLatin1().data());  // REMOVE Tim.

    QStringList sl = s.split('.');
    QValidator::State state;
    QValidator::State rv = QValidator::Acceptable;
    // "By default, the pos parameter is not used by this [QIntValidator] validator."
    int dpos = 0;

    if (sl.size() != 3)
    {
        printf("validate bbt string:%s sections:%d != 3\n", s.toLatin1().data(), sl.size());
        return QValidator::Invalid;
    }

    int tb = sigmap.ticksBeat(_pos.tick());
    unsigned tm = sigmap.ticksMeasure(_pos.tick());
    int bm = tm / tb;

    validator->setRange(1, 9999);
    //printf("validate substring 0:%s\n", sl[0].toLatin1().data());  // REMOVE Tim.
    // Special hack because validator says 0000 is intermediate.
    if (sl[0] == "0000")
        return QValidator::Invalid;
    state = validator->validate(sl[0], dpos);
    if (state == QValidator::Invalid)
        return state;
    if (state == QValidator::Intermediate)
        rv = state;

    validator->setRange(1, bm);
    //printf("validate substring 1:%s\n", sl[1].toLatin1().data());  // REMOVE Tim.
    // Special hack because validator says 00 is intermediate.
    if (sl[1] == "00")
        return QValidator::Invalid;
    state = validator->validate(sl[1], dpos);
    if (state == QValidator::Invalid)
        return state;
    if (state == QValidator::Intermediate)
        rv = state;

    validator->setRange(0, tb - 1);
    //printf("validate substring 2:%s\n", sl[2].toLatin1().data());  // REMOVE Tim.
    state = validator->validate(sl[2], dpos);
    if (state == QValidator::Invalid)
        return state;
    if (state == QValidator::Intermediate)
        rv = state;
    return rv;
}

//---------------------------------------------------------
//   curSegment
//---------------------------------------------------------

int PosEdit::curSegment() const
{
    QLineEdit* le = lineEdit();
    int pos = le->cursorPosition();
    int segment = -1;

    if (pos >= 0 && pos <= 4)
        segment = 0;
    else if (pos >= 5 && pos <= 7)
        segment = 1;
    else if (pos >= 8)
        segment = 2;
    else
        printf("curSegment = -1, pos %d\n", pos);

    return segment;
}

//---------------------------------------------------------
//   stepBy
//---------------------------------------------------------

void PosEdit::stepBy(int steps)
{
    int segment = curSegment();
    int selPos;
    int selLen;

    bool changed = false;

    int bar, beat, tick;
    _pos.mbt(&bar, &beat, &tick);

    int tb = sigmap.ticksBeat(_pos.tick());
    unsigned tm = sigmap.ticksMeasure(_pos.tick());
    int bm = tm / tb;

    switch (segment)
    {
    case 0:
        bar += steps;
        if (bar < 0)
            bar = 0;
        selPos = 0;
        selLen = 4;
        break;
    case 1:
        beat += steps;
        if (beat < 0)
            beat = 0;
        else if (beat >= bm)
            beat = bm - 1;
        selPos = 5;
        selLen = 2;
        break;
    case 2:
        tick += steps;
        if (tick < 0)
            tick = 0;
        else if (tick >= tb)
            tick = tb - 1;
        selPos = 8;
        selLen = 3;
        break;
    default:
        return;
    }
    Pos newPos(bar, beat, tick);
    if (!(newPos == _pos))
    {
        changed = true;
        _pos = newPos;
    }
    if (changed)
    {
        updateValue();
        emit valueChanged(_pos);
    }
    lineEdit()->setSelection(selPos, selLen);
}

//---------------------------------------------------------
//   paintEvent
//---------------------------------------------------------

void PosEdit::paintEvent(QPaintEvent* event)
{
    if (!initialized)
        updateValue();
    initialized = true;
    QAbstractSpinBox::paintEvent(event);
}

//---------------------------------------------------------
//   finishEdit
//---------------------------------------------------------

void PosEdit::finishEdit()
{
    // If our validator did its job correctly, the entire line edit text should be valid now...

    bool changed = false;
    QStringList sl = text().split('.');

    if (sl.size() != 3)
    {
        printf("finishEdit bbt string:%s sections:%d != 3\n", text().toLatin1().data(), sl.size());
        return;
    }

    Pos newPos(sl[0].toInt() - 1, sl[1].toInt() - 1, sl[2].toInt());
    if (!(newPos == _pos))
    {
        changed = true;
        _pos = newPos;
    }

    if (changed)
    {
        //updateValue();
        emit valueChanged(_pos);
    }
}
