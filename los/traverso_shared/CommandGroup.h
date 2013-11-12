//=========================================================
//  LOS
//  Libre Octave Studio
//
//
//
//  (C) Copyright 2011 Remon Sijrier
//=========================================================

#ifndef OOM_COMMAND_GROUP_H
#define OOM_COMMAND_GROUP_H

#include "OOMCommand.h"

#include <QList>

class CommandGroup : public LOSCommand
{
    Q_OBJECT
public :
    CommandGroup(const QString& des)
        : LOSCommand(des)
    {
    }
    ~CommandGroup() override;

    void addCommand(LOSCommand* const cmd)
    {
        Q_ASSERT(cmd != nullptr);
        fCommands.append(cmd);
    }

protected:
    int doAction() override;
    int undoAction() override;

private:
    QList<LOSCommand*> fCommands;
};

#endif
