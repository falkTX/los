//=========================================================
//  OOMidi
//  OpenOctave Midi and Audio Editor
//    $Id: menutitleitem.h,v 1.1.2.1 2009/06/10 00:34:59 terminator356 Exp $
//  (C) Copyright 1999-2001 Werner Schweer (ws@seh.de)
//=========================================================

#include "menutitleitem.h"

#include <QtGui/QLabel>

//---------------------------------------------------------
//   MenuTitleItem
//---------------------------------------------------------

MenuTitleItem::MenuTitleItem(const QString& ss, QWidget* parent)
: QWidgetAction(parent)
{
    s = ss;
    // Don't allow to click on it.
    setEnabled(false);
    // Just to be safe, set to -1 instead of default 0.
    setData(-1);
}

QWidget* MenuTitleItem::createWidget(QWidget *parent)
{
    QLabel* l = new QLabel(s, parent);
    l->setAlignment(Qt::AlignCenter);
    return l;
}
