//===========================================================
//  LOS
//  Libre Octave Studio
//  (C) Copyright 2011 Andrew Williams & Christopher Cherrett
//===========================================================

#ifndef _OOM_TRACKHEADER_H_
#define _OOM_TRACKHEADER_H_

#define CtrlIDRole Qt::UserRole+2

#include "ui_trackheaderbase.h"
#include "globaldefs.hpp"
#include <QHash>
#include <QList>

class Knob;
class QAction;
class QSize;
class QMouseEvent;
class QResizeEvent;
class QPoint;
class Slider;
class Meter;
class MidiTrack;

class TrackHeader : public QFrame, public Ui::TrackHeaderBase
{
    Q_OBJECT
    Q_PROPERTY(bool selected READ isSelected WRITE setSelected)

    MidiTrack* m_track;
    bool resizeFlag;
    bool m_selected;
    bool m_midiDetect;
    double panVal;
    double volume;
    QPoint m_startPos;
    int startY;
    int curY;
    bool inHeartBeat;
    bool m_editing;
    bool m_processEvents;
    bool m_meterVisible;
    bool m_sliderVisible;
    bool m_toolsVisible;
    bool m_nopopulate;
    QHash<int, QString> m_selectedStyle;
    QHash<int, QString> m_style;
    QList<Meter*> meter;

    void setupStyles();
    bool eventFilter(QObject *obj, QEvent *event);

private slots:
    void heartBeat();
    void toggleRecord(bool);
    void toggleMute(bool);
    void toggleSolo(bool);
    void toggleOffState(bool);
    void toggleReminder1(bool);
    void toggleReminder2(bool);
    void toggleReminder3(bool);
    void updateTrackName();
    void generatePopupMenu();

    void resetPeaks(bool);
    void resetPeaksOnPlay(bool);
    void updateSelection(bool shift = false);
    void setEditing(bool edit = true)
    {
        m_editing = edit;
    }
    void generateInstrumentMenu();
    void instrumentChangeRequested(qint64, const QString&, int);

public slots:
    void songChanged(int);
    void stopProcessing();
    void startProcessing();
    void setSelected(bool, bool force = false);
    void newTrackAdded(qint64);

protected:
    enum
    {
        NORMAL, START_DRAG, DRAG, RESIZE
    } mode;

    virtual void mousePressEvent(QMouseEvent*);
    virtual void mouseMoveEvent(QMouseEvent*);
    virtual void mouseReleaseEvent(QMouseEvent*);
    virtual void resizeEvent(QResizeEvent*);

signals:
    void selectionChanged(MidiTrack*);
    void trackInserted();
    void trackHeightChanged();

public:
    TrackHeader(MidiTrack* track, QWidget* parent = 0);
    virtual ~TrackHeader();
    bool isSelected();
    bool isEditing()
    {
        return m_editing;
    }
    MidiTrack* track()
    {
        return m_track;
    }
    void setTrack(MidiTrack*);
};

#endif
