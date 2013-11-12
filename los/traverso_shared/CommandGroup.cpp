//=========================================================
//  LOS
//  Libre Octave Studio
//
//
//
//  (C) Copyright 2011 Remon Sijrier
//=========================================================

#include "CommandGroup.h"

CommandGroup::~ CommandGroup()
{
    foreach (LOSCommand* cmd, fCommands)
        delete cmd;
}

int CommandGroup::doAction()
{
    foreach(LOSCommand* cmd, fCommands)
        cmd->doAction();

    return 1;
}

int CommandGroup::undoAction()
{
    foreach(LOSCommand* cmd, fCommands)
        cmd->undoAction();

    return 1;
}

// eof

