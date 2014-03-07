//===========================================================
//  LOS
//  Libre Octave Studio
//  (C) Copyright 2011 Andrew Williams & Christopher Cherrett
//===========================================================

#ifndef _OOM_CREATE_TRACKS_DIALOG_
#define _OOM_CREATE_TRACKS_DIALOG_

#include "ui_createtrackbase.h"
#include <QMap>
#include "TrackManager.h"

#define CTDInstrumentNameRole Qt::UserRole+5
#define CTDDeviceTypeRole Qt::UserRole+6
#define CTDDeviceNameRole Qt::UserRole+7

class QShowEvent;
class QSize;
class Knob;
class DoubleLabel;

class CreateTrackDialog : public QDialog,
                          public Ui::CreateTrackBase
{
    Q_OBJECT

public:
    CreateTrackDialog(int pos = -1, QWidget* parent = 0);
    CreateTrackDialog(VirtualTrack** vt, int pos = -1, QWidget* parent = 0);

signals:
    void trackAdded(qint64);

protected:
    void showEvent(QShowEvent*) /*override*/;

private slots:
    void addTrack();
    void cancelSelected();
    void updateInputSelected(bool);
    void updateOutputSelected(bool);
    void updateInstrument(int);
    void trackNameEdited();

    void showInputSettings();

private:
    const int m_insertPosition;

    VirtualTrack* m_vtrack;

    int m_width;
    int m_height;

    bool m_createMidiInputDevice;
    bool m_createMidiOutputDevice;

    QMap<int, QString> m_currentMidiInputList;
    QMap<int, QString> m_currentMidiOutputList;
    QStringList m_currentInput;
    QStringList m_currentOutput;

    void populateInputList();
    void populateOutputList();

    void populateMidiInputList();
    void populateMidiOutputList();
    void populateInstrumentList();

    void updateVisibleElements();

    void initDefaults();
};

#endif
