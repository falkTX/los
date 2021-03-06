//=========================================================
//  LOS
//  Libre Octave Studio
//    $Id: sigscale.h,v 1.2 2004/01/11 18:55:37 wschweer Exp $
//  (C) Copyright 1999 Werner Schweer (ws@seh.de)
//=========================================================

#ifndef __SIGSCALE_H__
#define __SIGSCALE_H__

#include "view.h"

class AbstractMidiEditor;

//---------------------------------------------------------
//   SigScale
//    Time Signature Scale
//---------------------------------------------------------

class SigScale : public View
{
    Q_OBJECT
    int* raster;
    unsigned pos[4];
    int button;
	bool m_startCursorMove;
	bool m_movingCursor;

signals:
    void posChanged(unsigned, unsigned);

protected:
    virtual void pdraw(QPainter&, const QRect&);
    virtual void viewMousePressEvent(QMouseEvent* event);
    virtual void viewMouseMoveEvent(QMouseEvent* event);
    virtual void viewMouseReleaseEvent(QMouseEvent* event);
    virtual void leaveEvent(QEvent*e);

signals:
    void timeChanged(unsigned);

public slots:
    void setPos(int, unsigned, bool);

public:
    SigScale(int* raster, QWidget* parent, int xscale);
};
#endif

