//=========================================================
//  LOS
//  Libre Octave Studio
//  $Id: gconfig.h,v 1.12.2.10 2009/12/01 03:52:40 terminator356 Exp $
//
//  (C) Copyright 1999-2003 Werner Schweer (ws@seh.de)
//=========================================================

#ifndef __CONFIG_H__
#define __CONFIG_H__

#define NUM_PARTCOLORS 112
#define NUM_FONTS 7

#include <QColor>
#include <QFont>
#include <QRect>
#include <QString>
#include <QStringList>

//---------------------------------------------------------
//   GlobalConfigValues
//---------------------------------------------------------

struct GlobalConfigValues
{
    int globalAlphaBlend;
    QColor palette[16];
    QColor partColors[NUM_PARTCOLORS];
    QColor partWaveColors[NUM_PARTCOLORS];
    QColor partColorsAutomation[NUM_PARTCOLORS];
    QColor partWaveColorsAutomation[NUM_PARTCOLORS];
    QString partColorNames[NUM_PARTCOLORS];
    QColor transportHandleColor;
    QColor bigTimeForegroundColor;
    QColor bigTimeBackgroundColor;
    QColor waveEditBackgroundColor;
    QFont fonts[NUM_FONTS];
    QColor trackBg;
    QColor selectTrackBg;
    QColor selectTrackFg;

    QColor midiTrackLabelBg;
    QColor drumTrackLabelBg;
    QColor waveTrackLabelBg;
    QColor outputTrackLabelBg;
    QColor inputTrackLabelBg;
    QColor groupTrackLabelBg;
    QColor auxTrackLabelBg;
    QColor synthTrackLabelBg;

    QColor midiTrackBg;
    QColor drumTrackBg;
    QColor waveTrackBg;
    QColor outputTrackBg;
    QColor inputTrackBg;
    QColor groupTrackBg;
    QColor auxTrackBg;
    QColor synthTrackBg;

    QColor partCanvasBg;
    QColor ctrlGraphFg;

    int division;
    int rtcTicks;
    int minMeter;
    double minSlider;
    bool freewheelMode;
    int guiRefresh;
    QString userInstrumentsDir;

    bool extendedMidi; // extended smf format
    int midiDivision; // division for smf export
    QString copyright; // copyright string for smf export
    int smfFormat; // smf export file type
    bool exp2ByteTimeSigs; // Export 2 byte time sigs instead of 4 bytes
    bool expOptimNoteOffs; // Save space by replacing note offs with note on velocity 0
    bool importMidiSplitParts; // Split imported tracks into multiple parts.

    // 0 - start with last song
    // 1 - start with default template
    // 2 - start with song
    int startMode;

    QString startSong; // path for start song
    int guiDivision; // division for tick display

    QRect geometryMain;
    QRect geometryTransport;
    QRect geometryBigTime;
    QRect geometryPerformer;
    QRect geometryDrumedit;
    bool transportVisible;
    bool bigTimeVisible;
    bool markerVisible;

    int canvasShowPartType; // 1 - names, 2 events
    int canvasShowPartEvent; //
    bool canvasShowGrid;
    QString canvasBgPixmap;
    QStringList canvasCustomBgList;
    QString styleSheetFile;
    QString style;

    QString externalWavEditor;
    bool useOldStyleStopShortCut;
    bool moveArmedCheckBox;
    bool useOutputLimiter;
    int dummyAudioSampleRate;
    int dummyAudioBufSize;
    QString projectBaseFolder;
    bool projectStoreInFolder;
    bool useProjectSaveDialog;

    int audioRaster;
    int midiRaster;
    bool useAutoCrossFades;
};

extern GlobalConfigValues config;

#endif

