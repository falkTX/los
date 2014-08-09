//===========================================================
//  LOS
//  Libre Octave Studio
//  (C) Copyright 2011 Andrew Williams & Christopher Cherrett
//===========================================================

#include <fastlog.h>
#include <math.h>

#include "gconfig.h"
#include "globals.h"
#include "config.h"
#include "track.h"
#include "app.h"
#include "song.h"
#include "audio.h"
#include "knob.h"
#include "popupmenu.h"
#include "globals.h"
#include "icons.h"
#include "shortcuts.h"
#include "scrollscale.h"
#include "xml.h"
#include "midi.h"
#include "mididev.h"
#include "midiport.h"
#include "midiseq.h"
#include "midictrl.h"
#include "comment.h"
#include "node.h"
#include "instruments/minstrument.h"
#include "Composer.h"
#include "event.h"
#include "midimonitor.h"
#include "ComposerCanvas.h"
#include "trackheader.h"
#include "slider.h"
#include "meter.h"
#include "CreateTrackDialog.h"
#include "TrackInstrumentMenu.h"

#include <QDrag>
#include <QMessageBox>
#include <QMimeData>

static QString styletemplate = "QLineEdit { border-width:1px; border-radius: 0px; border-image: url(:/images/frame.png) 4; border-top-color: #1f1f22; border-bottom-color: #505050; color: #%1; background-color: #%2; font-family: fixed-width; font-weight: bold; font-size: 15px; padding-left: 15px; }";
static QString trackHeaderStyle = "QFrame#TrackHeader { border-bottom: 1px solid #888888; border-right: 1px solid #888888; border-left: 1px solid #888888; background-color: #2e2e2e; }";
static QString trackHeaderStyleSelected = "QFrame#TrackHeader { border-bottom: 1px solid #888888; border-right: 1px solid #888888; border-left: 1px solid #888888; background-color: #171717; }";
static QString lineStyleTemplate = "QFrame { border: 0px; background-color: %1; }";

static const int TRACK_HEIGHT_3 = 100;
static const int TRACK_HEIGHT_4 = 180;
static const int TRACK_HEIGHT_5 = 320;
static const int TRACK_HEIGHT_6 = 640;

TrackHeader::TrackHeader(MidiTrack* t, QWidget* parent)
: QFrame(parent)
{
    setupUi(this);
    m_track = 0;
    setupStyles();
    resizeFlag = false;
    mode = NORMAL;
    inHeartBeat = true;
    m_editing = false;
    m_midiDetect = false;
    m_processEvents = true;
    m_meterVisible = true;
    m_sliderVisible = true;
    m_toolsVisible = false;
    m_nopopulate = false;
    panVal = 0.0;
    volume = 0.0;
    setObjectName("TrackHeader");
    setFrameStyle(QFrame::StyledPanel|QFrame::Raised);
    m_buttonHBox->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    m_buttonVBox->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    m_panBox->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    setAcceptDrops(false);
    setMouseTracking(true);

    m_trackName->installEventFilter(this);
    m_trackName->setAcceptDrops(false);
    m_btnSolo->setAcceptDrops(false);
    m_btnSolo->setIcon(*solo_trackIconSet3);
    m_btnRecord->setAcceptDrops(false);
    m_btnRecord->setIcon(*record_trackIconSet3);
    m_btnMute->setAcceptDrops(false);
    m_btnMute->setIcon(*mute_trackIconSet3);
    m_btnInstrument->setAcceptDrops(false);
    m_btnReminder1->setAcceptDrops(false);
    m_btnReminder2->setAcceptDrops(false);
    m_btnReminder3->setAcceptDrops(false);
    m_btnReminder1->setIcon(*reminder1IconSet3);
    m_btnReminder2->setIcon(*reminder2IconSet3);
    m_btnReminder3->setIcon(*reminder3IconSet3);

    setTrack(t);

    connect(m_trackName, SIGNAL(editingFinished()), this, SLOT(updateTrackName()));
    connect(m_trackName, SIGNAL(returnPressed()), this, SLOT(updateTrackName()));
    connect(m_trackName, SIGNAL(textEdited(QString)), this, SLOT(setEditing()));
    connect(m_btnRecord, SIGNAL(toggled(bool)), this, SLOT(toggleRecord(bool)));
    connect(m_btnMute, SIGNAL(toggled(bool)), this, SLOT(toggleMute(bool)));
    connect(m_btnSolo, SIGNAL(toggled(bool)), this, SLOT(toggleSolo(bool)));
    connect(m_btnReminder1, SIGNAL(toggled(bool)), this, SLOT(toggleReminder1(bool)));
    connect(m_btnReminder2, SIGNAL(toggled(bool)), this, SLOT(toggleReminder2(bool)));
    connect(m_btnReminder3, SIGNAL(toggled(bool)), this, SLOT(toggleReminder3(bool)));
    connect(m_btnInstrument, SIGNAL(clicked()), this, SLOT(generateInstrumentMenu()));
    //Let header list control this for now
    //connect(song, SIGNAL(songChanged(int)), this, SLOT(songChanged(int)));
    connect(song, SIGNAL(playChanged(bool)), this, SLOT(resetPeaksOnPlay(bool)));
    connect(heartBeatTimer, SIGNAL(timeout()), SLOT(heartBeat()));
    inHeartBeat = false;
}

TrackHeader::~TrackHeader()
{
    m_processEvents = false;
}

//Public member functions

void TrackHeader::stopProcessing()/*{{{*/
{
    m_processEvents = false;
}/*}}}*/

void TrackHeader::startProcessing()/*{{{*/
{
    m_processEvents = true;
}/*}}}*/

bool TrackHeader::isSelected()/*{{{*/
{
    if(!m_track)
        return false;
    return m_track->selected();
}/*}}}*/

void TrackHeader::setTrack(MidiTrack* track)/*{{{*/
{
    m_processEvents = false;

    Meter* m;
    while(!meter.isEmpty() && (m = meter.takeAt(0)) != 0)
    {
        //printf("Removing meter\n");
        m->hide();
        delete m;
    }
    m_track = track;
    if(!m_track || !track)
        return;

    m_trackName->setText(m_track->name());
    m_trackName->setReadOnly(true);

    setSelected(m_track->selected(), true);
    if(m_track->height() < MIN_TRACKHEIGHT)
    {
        setFixedHeight(MIN_TRACKHEIGHT);
        m_track->setHeight(MIN_TRACKHEIGHT);
    }
    else
    {
        setFixedHeight(m_track->height());
    }

    {
        MidiPort *mp = losMidiPorts.value(((MidiTrack*)m_track)->outPortId());
        m_btnInstrument->setIcon(*instrument_trackIconSet3);
        if(mp)
            m_btnInstrument->setToolTip(QString(tr("Change Instrument: ")).append(mp->instrument()->iname()));
        else
            m_btnInstrument->setToolTip(tr("Change Instrument"));
    }

    {
        m_btnRecord->setIcon(*record_trackIconSet3);
        m_btnRecord->setToolTip(tr("Record"));
    }

    m_btnRecord->blockSignals(true);
    m_btnRecord->setChecked(m_track->recordFlag());
    m_btnRecord->blockSignals(false);

    m_btnMute->blockSignals(true);
    m_btnMute->setChecked(m_track->mute());
    m_btnMute->blockSignals(false);

    m_btnSolo->blockSignals(true);
    m_btnSolo->setChecked(m_track->solo());
    m_btnSolo->blockSignals(false);

    m_btnReminder1->blockSignals(true);
    m_btnReminder1->setChecked(m_track->getReminder1());
    m_btnReminder1->blockSignals(false);

    m_btnReminder2->blockSignals(true);
    m_btnReminder2->setChecked(m_track->getReminder2());
    m_btnReminder2->blockSignals(false);

    m_btnReminder3->blockSignals(true);
    m_btnReminder3->setChecked(m_track->getReminder3());
    m_btnReminder3->blockSignals(false);

    m_trackName->blockSignals(true);
    m_trackName->setText(m_track->name());
    m_trackName->blockSignals(false);
    setSelected(m_track->selected());
    //setProperty("selected", m_track->selected());
    if(m_track->height() < MIN_TRACKHEIGHT)
    {
        setFixedHeight(MIN_TRACKHEIGHT);
        m_track->setHeight(MIN_TRACKHEIGHT);
    }
    else
    {
        setFixedHeight(m_track->height());
    }
    m_meterVisible = m_track->height() >= MIN_TRACKHEIGHT_VU;
    m_sliderVisible = m_track->height() >= MIN_TRACKHEIGHT_SLIDER;
    m_toolsVisible = (m_track->height() >= MIN_TRACKHEIGHT_TOOLS);

    foreach(Meter* m, meter)
        m->setVisible(m_meterVisible);

    m_processEvents = true;
    //songChanged(-1);
}/*}}}*/

//Public slots

void TrackHeader::setSelected(bool sel, bool force)/*{{{*/
{
    bool oldsel = m_selected;
    if(!m_track)
    {
        m_selected = false;
    }
    else
    {
        m_selected = sel;
        if(m_track->selected() != sel)
            m_track->setSelected(sel);
    }
    if((m_selected != oldsel) || force)
    {
        if(m_selected)
        {
            bool usePixmap = false;
            QColor sliderBgColor = g_trackColorListSelected.value(0);/*{{{*/
            switch(vuColorStrip)
            {
                case 0:
                    sliderBgColor = g_trackColorListSelected.value(0);
                break;
                case 1:
                    //if(width() != m_width)
                    //    m_scaledPixmap_w = m_pixmap_w->scaled(width(), 1, Qt::IgnoreAspectRatio);
                    //m_width = width();
                    //myPen.setBrush(m_scaledPixmap_w);
                    //myPen.setBrush(m_trackColor);
                    sliderBgColor = g_trackColorListSelected.value(0);
                    usePixmap = true;
                break;
                case 2:
                    sliderBgColor = QColor(0,166,172);
                    //myPen.setBrush(QColor(0,166,172));//solid blue
                break;
                case 3:
                    sliderBgColor = QColor(131,131,131);
                    //myPen.setBrush(QColor(131,131,131));//solid grey
                break;
                default:
                    sliderBgColor = g_trackColorListSelected.value(0);
                    //myPen.setBrush(m_trackColor);
                break;
            }/*}}}*/
            //m_trackName->setFont();
            m_trackName->setStyleSheet(m_selectedStyle[0]);
            setStyleSheet(trackHeaderStyleSelected);

            m_colorLine->setStyleSheet(lineStyleTemplate.arg(g_trackColorListLine.value(0).name()));
        }
        else
        {
            m_trackName->setStyleSheet(m_style[0]);
            setStyleSheet(trackHeaderStyle);

            m_colorLine->setStyleSheet(lineStyleTemplate.arg(g_trackColorListLine.value(0).name()));
            //m_strip->setStyleSheet("QFrame {background-color: blue;}");
        }
    }
}/*}}}*/

void TrackHeader::songChanged(int flags)/*{{{*/
{
    if(!m_track || !m_processEvents)
        return;

    if (flags & SC_TRACK_MODIFIED)/*{{{*/
    {
        //printf("TrackHeader::songChanged SC_TRACK_MODIFIED updating all\n");
        m_btnRecord->blockSignals(true);
        m_btnRecord->setChecked(m_track->recordFlag());
        m_btnRecord->blockSignals(false);

        m_btnMute->blockSignals(true);
        m_btnMute->setChecked(m_track->mute());
        m_btnMute->blockSignals(false);

        m_btnSolo->blockSignals(true);
        m_btnSolo->setChecked(m_track->solo());
        m_btnSolo->blockSignals(false);

        m_btnReminder1->blockSignals(true);
        m_btnReminder1->setChecked(m_track->getReminder1());
        m_btnReminder1->blockSignals(false);

        m_btnReminder2->blockSignals(true);
        m_btnReminder2->setChecked(m_track->getReminder2());
        m_btnReminder2->blockSignals(false);

        m_btnReminder3->blockSignals(true);
        m_btnReminder3->setChecked(m_track->getReminder3());
        m_btnReminder3->blockSignals(false);

        m_trackName->blockSignals(true);
        m_trackName->setText(m_track->name());
        m_trackName->blockSignals(false);

        setSelected(m_track->selected());

        if(m_track->height() < MIN_TRACKHEIGHT)
        {
            setFixedHeight(MIN_TRACKHEIGHT);
            m_track->setHeight(MIN_TRACKHEIGHT);
        }
        else
        {
            setFixedHeight(m_track->height());
        }
        return;
    }/*}}}*/
    if (flags & SC_MUTE)
    {
        //printf("TrackHeader::songChanged SC_MUTE\n");
        m_btnMute->blockSignals(true);
        m_btnMute->setChecked(m_track->mute());
        m_btnMute->blockSignals(false);
    }
    if (flags & SC_SOLO)
    {
        //printf("TrackHeader::songChanged SC_SOLO\n");
        m_btnSolo->blockSignals(true);
        m_btnSolo->setChecked(m_track->solo());
        m_btnSolo->blockSignals(false);
    }
    if (flags & SC_RECFLAG)
    {
        //printf("TrackHeader::songChanged SC_RECFLAG\n");
        m_btnRecord->blockSignals(true);
        m_btnRecord->setChecked(m_track->recordFlag());
        m_btnRecord->blockSignals(false);
    }
    if (flags & SC_MIDI_TRACK_PROP)
    {
        //printf("TrackHeader::songChanged SC_MIDI_TRACK_PROP\n");
    }
    if (flags & SC_SELECTION)
    {
        //printf("TrackHeader::songChanged SC_SELECTION\n");
        setSelected(m_track->selected());
    }
}/*}}}*/

//Private slots
void TrackHeader::heartBeat()/*{{{*/
{
    if(!m_track || inHeartBeat || !m_processEvents)
        return;
    if(song->invalid)
        return;
    inHeartBeat = true;

    {
        /*int act = track->activity();
        double dact = double(act) * (m_slider->value() / 127.0);

        if ((int) dact > track->lastActivity())
            track->setLastActivity((int) dact);

        foreach(Meter* m, meter)
            m->setVal(dact, track->lastActivity(), false);

        // Gives reasonable decay with gui update set to 20/sec.
        if (act)
            track->setActivity((int) ((double) act * 0.8));*/

        //int outChannel = track->outChannel();
        int outPort = m_track->outPort();

        MidiPort* mp = &midiPorts[outPort];

        // Check for detection of midi general activity on chosen channels...
        if(m_track->recordFlag())
        {
            //int mpt = 0;
            //RouteList* rl = m_track->inRoutes();

#if 0
            ciRoute r = rl->begin();
            for (; r != rl->end(); ++r)
            {
                if (!r->isValid() || (r->type != Route::MIDI_PORT_ROUTE))
                    continue;

                // NOTE: TODO: Code for channelless events like sysex,
                // ** IF we end up using the 'special channel 17' method.
                if (r->channel == -1 || r->channel == 0)
                    continue;

                // No port assigned to the device?
                mpt = r->midiPort;
                if (mpt < 0 || mpt >= kMaxMidiPorts)
                    continue;

                if (midiPorts[mpt].syncInfo().actDetectBits() & r->channel)
                {
                    if (!m_midiDetect)
                    {
                        m_midiDetect = true;
                        m_btnInstrument->setIcon(QIcon(*instrument_track_ActiveIcon));
                        //m_btnAutomation->setIcon(QIcon(*input_indicator_OnIcon));
                    }
                    break;
                }
            }
            // No activity detected?
            if (r == rl->end())
            {
                if (m_midiDetect)
                {
                    m_midiDetect = false;
                    m_btnInstrument->setIcon(*instrument_trackIconSet3);
                    /*if (m_track->wantsAutomation())
                        m_btnAutomation->setIcon(*automation_trackIconSet3);
                    else
                        m_btnAutomation->setIcon(QIcon(*input_indicator_OffIcon));*/
                }
            }
#endif
        }
        else
        {
            m_btnInstrument->setIcon(*instrument_trackIconSet3);
            /*if (m_track->wantsAutomation())
                m_btnAutomation->setIcon(*automation_trackIconSet3);
            else
                m_btnAutomation->setIcon(QIcon(*input_indicator_OffIcon));*/
        }
        if(mp)
            m_btnInstrument->setToolTip(QString(tr("Change Instrument: ")).append(mp->instrument()->iname()));
        else
            m_btnInstrument->setToolTip(tr("Change Instrument"));
    }

    inHeartBeat = false;
}/*}}}*/

void TrackHeader::generatePopupMenu()/*{{{*/
{
    if(!m_track || !m_processEvents)
        return;
    MidiTrackList selectedTracksList = song->getSelectedTracks();
    bool multipleSelectedTracks = false;
    if (selectedTracksList.size() > 1)
    {
        multipleSelectedTracks = true;
    }

    QMenu* p = new QMenu;
    //Part Color menu
    QMenu* colorPopup = p->addMenu(tr("Default Part Color"));/*{{{*/

    QMenu* colorSub;
    for (int i = 0; i < NUM_PARTCOLORS; ++i)
    {
        QString colorname(config.partColorNames[i]);
        if(colorname.contains("menu:", Qt::CaseSensitive))
        {
            colorSub = colorPopup->addMenu(colorname.replace("menu:", ""));
        }
        else
        {
            if(m_track->getDefaultPartColor() == i)
            {
                colorname = QString(config.partColorNames[i]);
                colorPopup->setIcon(ComposerCanvas::colorRect(config.partColors[i], config.partWaveColors[i], 80, 80, true));
                colorPopup->setTitle(colorSub->title()+": "+colorname);

                colorname = QString("* "+config.partColorNames[i]);
                QAction *act_color = colorSub->addAction(ComposerCanvas::colorRect(config.partColors[i], config.partWaveColors[i], 80, 80, true), colorname);
                act_color->setData(20 + i);
            }
            else
            {
                colorname = QString("     "+config.partColorNames[i]);
                QAction *act_color = colorSub->addAction(ComposerCanvas::colorRect(config.partColors[i], config.partWaveColors[i], 80, 80), colorname);
                act_color->setData(20 + i);
            }
        }
    }/*}}}*/

    //Add Track menu
    QAction* beforeTrack = p->addAction(tr("Add Track Before"));
    beforeTrack->setData(10000);
    QAction* afterTrack = p->addAction(tr("Add Track After"));
    afterTrack->setData(12000);

    {
        p->addAction(QIcon(*midi_edit_instrumentIcon), tr("Rename Track"))->setData(15);
        p->addSeparator();
        p->addAction(QIcon(*automation_clear_dataIcon), tr("Delete Track"))->setData(0);
        p->addSeparator();
    }

    QAction* selectAllAction = p->addAction(tr("Select All Tracks"));
    selectAllAction->setData(4);
    selectAllAction->setShortcut(shortcuts[SHRT_SEL_ALL_TRACK].key);

    QString currentTrackHeightString = tr("Track Height") + ": ";
    if (m_track->height() == DEFAULT_TRACKHEIGHT)
    {
        currentTrackHeightString += tr("Default");
    }
    if (m_track->height() == MIN_TRACKHEIGHT)
    {
        currentTrackHeightString += tr("Compact");
    }
    if (m_track->height() == TRACK_HEIGHT_3)
    {
        currentTrackHeightString += tr("3");
    }
    if (m_track->height() == TRACK_HEIGHT_4)
    {
        currentTrackHeightString += tr("4");
    }
    if (m_track->height() == TRACK_HEIGHT_5)
    {
        currentTrackHeightString += tr("5");
    }
    if (m_track->height() == TRACK_HEIGHT_6)
    {
        currentTrackHeightString += tr("6");
    }
    if (m_track->height() == los->composer->getCanvas()->height())
    {
        currentTrackHeightString += tr("Full Screen");
    }

    QMenu* trackHeightsMenu = p->addMenu(currentTrackHeightString);

    trackHeightsMenu->addAction(tr("Compact"))->setData(7);
    trackHeightsMenu->addAction(tr("Default"))->setData(6);
    trackHeightsMenu->addAction("3")->setData(8);
    trackHeightsMenu->addAction("4")->setData(9);
    trackHeightsMenu->addAction("5")->setData(10);
    trackHeightsMenu->addAction("6")->setData(11);
    trackHeightsMenu->addAction(tr("Full Screen"))->setData(12);
    if (selectedTracksList.size() > 1)
    {
        trackHeightsMenu->addAction(tr("Fit Selection in View"))->setData(13);
    }

    QAction* act = p->exec(QCursor::pos());
    if (act)
    {
        int n = act->data().toInt();
        switch (n)
        {
            case 0: // delete track
            {
                /*QString msg(tr("You are about to delete \n%1 \nAre you sure this is what you want?"));
                if(QMessageBox::question(this,
                    tr("Delete Track"),
                    msg.arg(multipleSelectedTracks ? "all selected tracks" : m_track->name()),
                    QMessageBox::Ok, QMessageBox::Cancel) == QMessageBox::Ok)
                {*/
                    if (multipleSelectedTracks)
                    {
                        trackManager->removeSelectedTracks();
                    /*	song->startUndo();
                        audio->msgRemoveTracks();
                        song->endUndo(SC_TRACK_REMOVED);
                        song->updateSoloStates();*/
                    }
                    else
                    {
                        trackManager->removeTrack(m_track->id());
                        //song->removeTrack(m_track);
                        //audio->msgUpdateSoloStates();
                    }
                //}
            }
            break;

            case 2:
            {
            }
            break;
            case 3:
            {
            }
            break;
            case 4:
            {
                MidiTrackList* tl = song->visibletracks();
                MidiTrackList selectedTracks = song->getSelectedTracks();
                bool select = true;
                if (selectedTracks.size() == tl->size())
                {
                    select = false;
                }

                for (iMidiTrack t = tl->begin(); t != tl->end(); ++t)
                {
                    (*t)->setSelected(select);
                }
                song->update(SC_SELECTION);
            }
            break;
            case 5:
            {
            }
            case 6:
            {
                if (multipleSelectedTracks)
                {
                    song->setTrackHeights(selectedTracksList, DEFAULT_TRACKHEIGHT);
                }
                else
                {
                    m_track->setHeight(DEFAULT_TRACKHEIGHT);
                    song->update(SC_TRACK_MODIFIED);
                }
                emit trackHeightChanged();
                break;
            }
            case 7:
            {
                if (multipleSelectedTracks)
                {
                    song->setTrackHeights(selectedTracksList, MIN_TRACKHEIGHT);
                }
                else
                {
                    m_track->setHeight(MIN_TRACKHEIGHT);
                    song->update(SC_TRACK_MODIFIED);
                }
                emit trackHeightChanged();
                break;
            }
            case 8:
            {
                if (multipleSelectedTracks)
                {
                    song->setTrackHeights(selectedTracksList, TRACK_HEIGHT_3);
                }
                else
                {
                    m_track->setHeight(TRACK_HEIGHT_3);
                    song->update(SC_TRACK_MODIFIED);
                }
                emit trackHeightChanged();
                break;
            }
            case 9:
            {
                if (multipleSelectedTracks)
                {
                    song->setTrackHeights(selectedTracksList, TRACK_HEIGHT_4);
                }
                else
                {
                    m_track->setHeight(TRACK_HEIGHT_4);
                    song->update(SC_TRACK_MODIFIED);
                }
                emit trackHeightChanged();
                break;
            }
            case 10:
            {
                if (multipleSelectedTracks)
                {
                    song->setTrackHeights(selectedTracksList, TRACK_HEIGHT_5);
                }
                else
                {
                    m_track->setHeight(TRACK_HEIGHT_5);
                    song->update(SC_TRACK_MODIFIED);
                }
                emit trackHeightChanged();
                break;
            }
            case 11:
            {
                if (multipleSelectedTracks)
                {
                    song->setTrackHeights(selectedTracksList, TRACK_HEIGHT_6);
                }
                else
                {
                    m_track->setHeight(TRACK_HEIGHT_6);
                    song->update(SC_TRACK_MODIFIED);
                }
                emit trackHeightChanged();
                break;
            }
            case 12:
            {
                int canvasHeight = los->composer->getCanvas()->height();

                if (multipleSelectedTracks)
                {
                    song->setTrackHeights(selectedTracksList, canvasHeight);
                    MidiTrack* firstSelectedTrack = *selectedTracksList.begin();
                    los->composer->verticalScrollSetYpos(los->composer->getCanvas()->track2Y(firstSelectedTrack));

                }
                else
                {
                    m_track->setHeight(canvasHeight);
                    song->update(SC_TRACK_MODIFIED);
                    los->composer->verticalScrollSetYpos(los->composer->getCanvas()->track2Y(m_track));
                }
                emit trackHeightChanged();
                break;
            }
            case 13:
            {
                int canvasHeight = los->composer->getCanvas()->height();
                if (multipleSelectedTracks)
                {
                    song->setTrackHeights(selectedTracksList, canvasHeight / selectedTracksList.size());
                    MidiTrack* firstSelectedTrack = *selectedTracksList.begin();
                    los->composer->verticalScrollSetYpos(los->composer->getCanvas()->track2Y(firstSelectedTrack));

                }
                else
                {
                    m_track->setHeight(canvasHeight);
                    song->update(SC_TRACK_MODIFIED);
                    los->composer->verticalScrollSetYpos(los->composer->getCanvas()->track2Y(m_track));
                }
                emit trackHeightChanged();
                break;
            }
            case 15:
            {
                if(m_track->name() != "Master")
                {
                    m_trackName->setReadOnly(false);
                    m_trackName->setFocus();
                    setEditing(true);
                }
                //if(!multipleSelectedTracks)
                //{
                //	renameTrack(m_track);
                //}
                break;
            }
            case 16:
            {
            }
            break;
            case 20 ... NUM_PARTCOLORS + 20:
            {
                int curColorIndex = n - 20;
                m_track->setDefaultPartColor(curColorIndex);
                break;
            }
            case 10000:
            {//Insert before
                int mypos = song->tracks()->index(m_track);
                CreateTrackDialog *ctdialog = new CreateTrackDialog(--mypos, this);
                connect(ctdialog, SIGNAL(trackAdded(qint64)), this, SLOT(newTrackAdded(qint64)));
                ctdialog->exec();

                break;
            }
            case 12000:
            {//Insert after
                int mypos = song->tracks()->index(m_track);
                CreateTrackDialog *ctdialog = new CreateTrackDialog(++mypos, this);
                connect(ctdialog, SIGNAL(trackAdded(qint64)), this, SLOT(newTrackAdded(qint64)));
                ctdialog->exec();
                break;
            }
            default:
                printf("action %d\n", n);
            break;
        }
    }
    delete trackHeightsMenu;
    delete p;
}/*}}}*/

void TrackHeader::newTrackAdded(qint64 id)
{
    MidiTrack* t = song->findTrackById(id);
    if (t)
    {
        emit selectionChanged(t);
        emit trackInserted();
        song->updateTrackViews();
    }
}

void TrackHeader::generateInstrumentMenu()/*{{{*/
{
    if(!m_track)
        return;

    QMenu* p = new QMenu(this);
    //p->setTearOffEnabled(true);
    TrackInstrumentMenu *imenu = new TrackInstrumentMenu(p, m_track);
    connect(imenu, SIGNAL(instrumentSelected(qint64, const QString&, int)), this, SLOT(instrumentChangeRequested(qint64, const QString&, int)));

    p->addAction(imenu);
    p->exec(QCursor::pos());

    delete p;
    updateSelection(false);
    //emit selectionChanged(m_track);
}/*}}}*/

void TrackHeader::instrumentChangeRequested(qint64 id, const QString& instrument, int type)
{
    trackManager->setTrackInstrument(id, instrument, type);
}

void TrackHeader::toggleRecord(bool state)/*{{{*/
{
    if(!m_track || !m_processEvents)
        return;

    {
        m_btnRecord->blockSignals(true);
        m_btnRecord->setChecked(state);
        m_btnRecord->blockSignals(false);
        song->setRecordFlag(m_track, state);
    }
    /*
    else if (button == Qt::RightButton)
    {
        // enable or disable ALL tracks of this type
        if (!m_track->isMidiTrack() && valid)
        {
            if (m_track->type() == Track::AUDIO_OUTPUT)
            {
                return;
            }
            WaveTrackList* wtl = song->waves();

            foreach(WaveTrack *wt, *wtl)
            {
                song->setRecordFlag(wt, val);
            }
        }
        else if(valid)
        {
            MidiTrackList* mtl = song->midis();

            foreach(MidiTrack *mt, *mtl)
            {
                song->setRecordFlag(mt, val);
            }
        }
        else
        {
            updateSelection(m_track, shift);
        }
    }
    */
}/*}}}*/

void TrackHeader::toggleMute(bool state)/*{{{*/
{
    if(!m_track || !m_processEvents)
        return;
    m_track->setMute(state);
    m_btnMute->blockSignals(true);
    m_btnMute->setChecked(state);
    m_btnMute->blockSignals(false);
    song->update(SC_MUTE);
}/*}}}*/

void TrackHeader::toggleSolo(bool state)/*{{{*/
{
    if(!m_track || !m_processEvents)
        return;
    audio->msgSetSolo(m_track, state);
    m_btnSolo->blockSignals(true);
    m_btnSolo->setChecked(state);
    m_btnSolo->blockSignals(false);
    song->update(SC_SOLO);
}/*}}}*/

void TrackHeader::toggleOffState(bool state)/*{{{*/
{
    if(!m_track || !m_processEvents)
        return;
    m_track->setOff(state);
}/*}}}*/

void TrackHeader::toggleReminder1(bool state)/*{{{*/
{
    if(!m_track || !m_processEvents)
        return;
    m_track->setReminder1(state);
}/*}}}*/

void TrackHeader::toggleReminder2(bool state)/*{{{*/
{
    if(!m_track || !m_processEvents)
        return;
    m_track->setReminder2(state);
}/*}}}*/

void TrackHeader::toggleReminder3(bool state)/*{{{*/
{
    if(!m_track || !m_processEvents)
        return;
    m_track->setReminder3(state);
}/*}}}*/

void TrackHeader::updateTrackName()/*{{{*/
{
    if(!m_track || !m_processEvents)
    {
        m_trackName->setReadOnly(true);
        return;
    }
    QString name = m_trackName->text();
    if(name.isEmpty())
    {
        //m_trackName->blockSignals(true);
        m_trackName->undo();//setText(m_track->name());
        //m_trackName->blockSignals(false);
        setEditing(false);
        m_trackName->setReadOnly(true);
        return;
    }
    if (name != m_track->name())
    {
        MidiTrackList* tl = song->tracks();
        for (iMidiTrack i = tl->begin(); i != tl->end(); ++i)
        {
            if ((*i)->name() == name)
            {
                QMessageBox::critical(this,
                        tr("LOS: bad trackname"),
                        tr("please choose a unique track name"),
                        QMessageBox::Ok,
                        Qt::NoButton,
                        Qt::NoButton);
                //m_trackName->blockSignals(true);
                //m_trackName->setText(m_track->name());
                //m_trackName->blockSignals(false);
                m_trackName->undo();//setText(m_track->name());
                setEditing(false);
                m_trackName->setReadOnly(true);
                return;
            }
        }
        MidiTrack* track = m_track->clone(false);
        m_track->setName(name);
        audio->msgChangeTrack(track, m_track);
    }
    m_trackName->setReadOnly(true);
    setEditing(false);
}/*}}}*/

void TrackHeader::resetPeaks(bool)/*{{{*/
{
    if(!m_track)
        return;

    m_track->resetPeaks();
}/*}}}*/

void TrackHeader::resetPeaksOnPlay(bool play)/*{{{*/
{
    if(!m_track)
        return;
    if(play)
        resetPeaks(play);
}/*}}}*/

//Private member functions

bool TrackHeader::eventFilter(QObject *obj, QEvent *event)/*{{{*/
{
    if(!m_processEvents)
        return true;
    /*if (event->type() & (QEvent::MouseButtonPress | QEvent::MouseMove | QEvent::MouseButtonRelease))
    {
        bool alltype = false;
        bool isname = false;
        if(obj == m_trackName)
        {
            isname = true;
            QLineEdit* tname = static_cast<QLineEdit*>(obj);
            if(tname && tname->isReadOnly())
            {
                alltype = true;
            }
        }
        if(alltype && isname)
        {
            QMouseEvent *mEvent = static_cast<QMouseEvent *>(event);
            mousePressEvent(mEvent);
            mode = NORMAL;
            return true;
            //return QObject::eventFilter(obj, event);
        }
    }
    */
    if (event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent *mEvent = static_cast<QMouseEvent *>(event);
        if(mEvent && mEvent->button() == Qt::LeftButton )
        {
            mousePressEvent(mEvent);
            mode = NORMAL;
        }
        else if(mEvent && mEvent->button() == Qt::RightButton && obj == m_trackName)
        {
            mousePressEvent(mEvent);
            mode = NORMAL;
        }
    }
    if(event->type() == QEvent::KeyPress)
    {
        QKeyEvent* kEvent = static_cast<QKeyEvent*>(event);
        if((kEvent->key() == Qt::Key_Return || kEvent->key() == Qt::Key_Enter) && m_editing)
        {
            updateTrackName();
        }
    }
    // standard event processing
    return QObject::eventFilter(obj, event);

}/*}}}*/

void TrackHeader::setupStyles()/*{{{*/
{
    m_style.insert(0, styletemplate.arg(QString("939393"), QString("1a1a1a")));
    m_selectedStyle.insert(0, styletemplate.arg(QString("01e6ee"), QString("0a0a0a")));
}/*}}}*/

void TrackHeader::updateSelection(bool shift)/*{{{*/
{
    if(!m_track)
        return;
    //printf("TrackHeader::updateSelection - shift; %d\n", shift);
    if (!shift)
    {
        song->deselectTracks();
        if(song->hasSelectedParts)
            song->deselectAllParts();
        setSelected(true);

        //record enable track if expected
        int recd = 0;
        MidiTrackList* tracks = song->visibletracks();
        MidiTrack* recTrack = 0;
        for (iMidiTrack t = tracks->begin(); t != tracks->end(); ++t)
        {
            if ((*t)->recordFlag())
            {
                if(!recTrack)
                    recTrack = *t;
                recd++;
            }
        }
        if (recd == 1 && config.moveArmedCheckBox)
        {
            //one rec enabled track, move rec enabled with selection
            song->setRecordFlag(recTrack, false);
            song->setRecordFlag(m_track, true);
        }
    }
    else
    {
        song->deselectAllParts();
        setSelected(true);
    }
    song->update(SC_SELECTION | SC_RECFLAG);
}/*}}}*/

//Protected events
//We overwrite these from QWidget to implement our own functionality

void TrackHeader::mousePressEvent(QMouseEvent* ev) //{{{
{
    if(!m_track || !m_processEvents)
        return;
    int button = ev->button();
    bool shift = ((QInputEvent*) ev)->modifiers() & Qt::ShiftModifier;

    if (button == Qt::LeftButton)
    {
        if (resizeFlag)
        {
            startY = ev->y();
            mode = RESIZE;
            return;
        }
        else
        {
            m_startPos = ev->pos();
            updateSelection(shift);
            if(m_track->name() != "Master")
                mode = START_DRAG;
        }
    }
    else if(button == Qt::RightButton)
    {
        generatePopupMenu();
    }
}/*}}}*/

void TrackHeader::mouseMoveEvent(QMouseEvent* ev)/*{{{*/
{
    if(!m_track || !m_processEvents)
        return;
    bool shift = ((QInputEvent*) ev)->modifiers() & Qt::ShiftModifier;
    if(shift)
    {
        resizeFlag = false;
        setCursor(QCursor(Qt::ArrowCursor));
        return;
    }
    if ((((QInputEvent*) ev)->modifiers() | ev->buttons()) == 0)
    {
        //QRect geo = geometry();
        QRect hotBox(0, m_track->height() - 2, width(), 2);
        //printf("HotBox: x: %d, y: %d, event pos x: %d, y: %d, geo bottom: %d\n", hotBox.x(), hotBox.y(), ev->x(), ev->y(), geo.bottom());
        if (hotBox.contains(ev->pos()))
        {
            //printf("Hit hotspot\n");
            if (!resizeFlag)
            {
                resizeFlag = true;
                setCursor(QCursor(Qt::SplitVCursor));
            }
        }
        else
        {
            resizeFlag = false;
            setCursor(QCursor(Qt::ArrowCursor));
        }
        return;
    }
    curY = ev->y();
    int delta = curY - startY;
    switch (mode)
    {
        case START_DRAG:
        {
            if ((ev->pos() - m_startPos).manhattanLength() < QApplication::startDragDistance())
                return;

            m_editing = true;
            mode = DRAG;
            QPoint hotSpot = ev->pos();
            int index = song->visibletracks()->index(m_track);

            QByteArray itemData;
            QDataStream dataStream(&itemData, QIODevice::WriteOnly);
            dataStream << m_track->name() << index << QPoint(hotSpot);

            QMimeData *mimeData = new QMimeData;
            mimeData->setData("los/x-trackinfo", itemData);
            mimeData->setText(m_track->name());

            QDrag *drag = new QDrag(this);
            drag->setMimeData(mimeData);
            drag->setPixmap(g_trackDragImageList.value(0));
            drag->setHotSpot(QPoint(80,20));//hotSpot);
            drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::MoveAction);
        }
        break;
        case NORMAL:
        case DRAG:
        break;
        case RESIZE:
        {
            if (m_track)
            {
                int h = m_track->height() + delta;
                startY = curY;
                if (h < MIN_TRACKHEIGHT)
                    h = MIN_TRACKHEIGHT;
                m_track->setHeight(h);
                song->update(SC_TRACK_MODIFIED);
            }
        }
        break;
    }
}/*}}}*/

void TrackHeader::mouseReleaseEvent(QMouseEvent*)/*{{{*/
{
    if(mode == RESIZE)
        emit trackHeightChanged();
    mode = NORMAL;
    setCursor(QCursor(Qt::ArrowCursor));
    m_editing = false;
    resizeFlag = false;
}/*}}}*/

void TrackHeader::resizeEvent(QResizeEvent* event)/*{{{*/
{
    //We will trap this to disappear widgets like vu's and volume slider
    //on the track header. For now we'll just pass it up the chain
    QSize size = event->size();
    if(m_track)
    {
        m_meterVisible = size.height() >= MIN_TRACKHEIGHT_VU;
        m_sliderVisible = size.height() >= MIN_TRACKHEIGHT_SLIDER;
        m_toolsVisible = (size.height() >= MIN_TRACKHEIGHT_TOOLS);
        foreach(Meter* m, meter)
            m->setVisible(m_meterVisible);
    }
    //QFrame::resizeEvent(event);
}/*}}}*/

