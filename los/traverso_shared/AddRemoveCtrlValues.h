//=========================================================
//  LOS
//  Libre Octave Studio
//
//    add ctrl value command class
//
//  (C) Copyright 2011 Remon Sijrier
//=========================================================

#ifndef ADD_CTRL_VALUES_H
#define ADD_CTRL_VALUES_H

#include "OOMCommand.h"

#include "ctrl.h"

class AddRemoveCtrlValues : public LOSCommand
{
    Q_OBJECT

public:
    AddRemoveCtrlValues(CtrlList* const cl, CtrlVal ctrlValues, const int type);
    AddRemoveCtrlValues(CtrlList* const cl, QList<CtrlVal> ctrlValues, const int type);
    AddRemoveCtrlValues(CtrlList* const cl, QList<CtrlVal> ctrlValues, QList<CtrlVal> newCtrlValues, const int type = MODIFY);

protected:
    int doAction() override;
    int undoAction() override;

private:
    const int fType;
    double fStartValue;
    CtrlList* fCtrlList;
    QList<CtrlVal> fCtrlValues;
    QList<CtrlVal> fNewCtrlValues;
};

#endif // ADD_CTRL_VALUE_H
