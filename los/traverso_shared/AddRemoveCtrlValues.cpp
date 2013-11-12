//=========================================================
//  LOS
//  Libre Octave Studio
//
//    add ctrl value command class
//
//  (C) Copyright 2011 Remon Sijrier
//=========================================================


#include "AddRemoveCtrlValues.h"

#include "ctrl.h"

AddRemoveCtrlValues::AddRemoveCtrlValues(CtrlList* const cl, CtrlVal ctrlValue, const int type)
    : LOSCommand(tr("Add Node")),
      fType(type),
      fStartValue(cl->value(0)),
      fCtrlList(cl)
{
    fCtrlValues << ctrlValue;
}

AddRemoveCtrlValues::AddRemoveCtrlValues(CtrlList* const cl, QList<CtrlVal> ctrlValues, const int type)
    : LOSCommand(tr("Add Nodes")),
      fType(type),
      fStartValue(cl->value(0)),
      fCtrlList(cl),
      fCtrlValues(ctrlValues)
{
}

AddRemoveCtrlValues::AddRemoveCtrlValues(CtrlList* const cl, QList<CtrlVal> ctrlValues, QList<CtrlVal> newCtrlValues, const int type)
    : LOSCommand(tr("Move Nodes")),
      fType(type),
      fStartValue(cl->value(0)),
      fCtrlList(cl),
      fCtrlValues(ctrlValues),
      fNewCtrlValues(newCtrlValues)
{
}

int AddRemoveCtrlValues::doAction()
{
    if (fType == ADD)
    {
        foreach (CtrlVal v, fCtrlValues)
            fCtrlList->add(v.getFrame(), v.val);
    }
    else if (fType == REMOVE)
    {
        foreach (CtrlVal v, fCtrlValues)
            fCtrlList->del(v.getFrame());

        fCtrlList->add(0, fStartValue);
    }
    else
    {
        // Delete the old
        foreach (CtrlVal v, fCtrlValues)
            fCtrlList->del(v.getFrame());

        // Add the new
        foreach (CtrlVal v, fNewCtrlValues)
            fCtrlList->add(v.getFrame(), v.val);
    }

    return 1;
}

int AddRemoveCtrlValues::undoAction()
{
    if (fType == ADD)
    {
        foreach (CtrlVal v, fCtrlValues)
            fCtrlList->del(v.getFrame());

        fCtrlList->add(0, fStartValue);
    }
    else if (fType == REMOVE)
    {
        foreach (CtrlVal v, fCtrlValues)
            fCtrlList->add(v.getFrame(), v.val);
    }
    else
    {
        // Delete the new
        foreach(CtrlVal v, fNewCtrlValues)
            fCtrlList->del(v.getFrame());

        // Add the old
        foreach(CtrlVal v, fCtrlValues)
            fCtrlList->add(v.getFrame(), v.val);
    }

    return 1;
}
