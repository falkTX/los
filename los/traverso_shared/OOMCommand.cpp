//=========================================================
//  LOS
//  Libre Octave Studio
//
//
//
//  (C) Copyright 2011 Remon Sijrier
//=========================================================

#include "OOMCommand.h"

LOSCommand::LOSCommand(const QString& des)
    : QUndoCommand(des),
      fDescription(des)
{
}

LOSCommand::~LOSCommand()
{}

void LOSCommand::processCommand(LOSCommand* cmd)
{
    Q_ASSERT(cmd);
    qDebug("processing %s\n", cmd->text().toUtf8().constData());

    cmd->redo();
}

int LOSCommand::doAction()
{
    return -1;
}

int LOSCommand::undoAction()
{
    return -1;
}
