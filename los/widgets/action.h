//=========================================================
//  OOMidi
//  OpenOctave Midi and Audio Editor
//    $Id: action.h,v 1.1.1.1.2.1 2008/01/19 13:33:46 wschweer Exp $
//  (C) Copyright 2001 Werner Schweer (ws@seh.de)
//=========================================================

#ifndef WIDGETS_ACTION_H_INCLUDED
#define WIDGETS_ACTION_H_INCLUDED

#include <QAction>

//---------------------------------------------------------
//   Action
//---------------------------------------------------------

class Action : public QAction
{
    Q_OBJECT

public:
    Action(QObject* parent, int i, const char* name = 0, bool toggle = false)
        : QAction(name, parent),
          _id(i)
    {
        setCheckable(toggle);
    }

    int id() const noexcept
    {
        return _id;
    }

    void setId(int i) noexcept
    {
        _id = i;
    }

private:
    int _id;
};

//---------------------------------------------------------

#endif // WIDGETS_ACTION_H_INCLUDED
