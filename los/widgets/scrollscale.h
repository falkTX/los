//=========================================================
//  LOS
//  Libre Octave Studio
//    $Id: scrollscale.h,v 1.2.2.3 2009/11/04 17:43:26 lunar_shuttle Exp $
//  (C) Copyright 1999 Werner Schweer (ws@seh.de)
//=========================================================

#ifndef __SCROLLSCALE_H__
#define __SCROLLSCALE_H__

#include <QSlider>

class QBoxLayout;
class QLabel;
class QResizeEvent;
class QScrollBar;
class QToolButton;

//---------------------------------------------------------
//   ScrollScale
//---------------------------------------------------------

class ScrollScale : public QWidget
{
    QSlider* scale;
    QScrollBar* scroll;
    int minVal, maxVal;
    float scaleVal, scaleMin, scaleMax;
    bool showMagFlag;
    QBoxLayout* box;
    bool noScale;
    bool pageButtons;
    int _page;
    int _pages;
    QToolButton* up;
    QToolButton* down;
    QLabel* pageNo;
    bool invers;
    double logbase;

    virtual void resizeEvent(QResizeEvent*);
    Q_OBJECT

private slots:
    void pageUp();
    void pageDown();

public slots:
    void setPos(unsigned);
    void setPosNoLimit(unsigned);
    void setMag(int);
    void setOffset(int val);
    void setScale(int);

signals:
    void scaleChanged(float);
    void lscaleChanged(int);
    void scrollChanged(int);
    void newPage(int);

public:
    ScrollScale(int, int, int, int max, Qt::Orientation,
            QWidget*, int min = 0, bool i = false, double vv = 10.0);

    int xmag() const
    {
        return scale->value();
    }

    void setXmag(float val)
    {
        scale->setValue(val);
    }
    void setRange(int, int);
    void showMag(bool);

    void setNoScale(bool flag)
    {
        noScale = flag;
    }
    void setPageButtons(bool flag);

    void setPage(int n)
    {
        _page = n;
    }

    int page() const
    {
        return _page;
    }

    int pages() const
    {
        return _pages;
    }
    void setPages(int n);
    int pos() const;
    int mag() const;

    float getScaleValue() const
    {
        return scaleVal;
    }

    void range(int* b, int* e) const
    {
        *b = minVal;
        *e = maxVal;
    }

    int offset();
    int pos2offset(int pos);
    static int getQuickZoomLevel(int mag);
    static int convertQuickZoomLevelToMag(int zoomlvl);
};

#endif

