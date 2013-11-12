//=========================================================
//  LOS
//  Libre Octave Studio
//
//
//
//  (C) Copyright 2011 Remon Sijrier
//=========================================================

#ifndef OOMCOMMAND_H
#define OOMCOMMAND_H

#include <QObject>
#include <QUndoCommand>
#include <QUndoStack>

class LOSCommand : public QObject,
                   public QUndoCommand
{
    Q_OBJECT

public:
    enum Type {
        ADD, REMOVE, MODIFY
    };

    LOSCommand(const QString& des = "No description set!");
    virtual ~LOSCommand();

    void undo()
    {
        undoAction();
    }

    void redo()
    {
        doAction();
    }

    static void processCommand(LOSCommand* cmd);

protected:
    QString fDescription;

    virtual int doAction();
    virtual int undoAction();

    friend class CommandGroup;
};


#endif


