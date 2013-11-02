//===========================================================
//  OOMidi
//  OpenOctave Midi and Audio Editor
//  (C) Copyright 2011 Andrew Williams & Christopher Cherrett
//===========================================================

#include "CreateTrackDialog.h"
#include "NameValidator.h"

#include "audio.h"
#include "audiodev.h"
#include "gconfig.h"
#include "genset.h"
#include "globals.h"
#include "icons.h"
#include "mididev.h"
#include "midiport.h"
#include "minstrument.h"
#include "song.h"
#include "track.h"

#include <QMessageBox>

CreateTrackDialog::CreateTrackDialog(int type, int pos, QWidget* parent)
    : QDialog(parent),
      m_insertType(type),
      m_insertPosition(pos),
      m_templateMode(false)
{
    initDefaults();
    m_vtrack = new VirtualTrack;
}

CreateTrackDialog::CreateTrackDialog(VirtualTrack** vt, int type, int pos, QWidget* parent)
    : QDialog(parent),
      m_insertType(type),
      m_insertPosition(pos),
      m_templateMode(true)
{
    initDefaults();
    m_vtrack = new VirtualTrack;
    *vt = m_vtrack;
}

void CreateTrackDialog::initDefaults()
{
    setupUi(this);

    m_createMidiInputDevice = false;
    m_createMidiOutputDevice = false;

    m_midiSameIO = false;
    m_instrumentLoaded = false;
    m_showJackAliases = -1;

    m_midiInPort = -1;
    m_midiOutPort = -1;
    m_instrumentMap = -1;
    m_existingMap = false;

    m_height = 290;
    m_width = 450;

    m_allChannelBit = (1 << MIDI_CHANNELS) - 1;
    txtName->setValidator(new NameValidator(this));

    cmbType->addItem(*addMidiIcon, tr("Midi"), Track::MIDI);
    cmbType->addItem(*addAudioIcon, tr("Audio"), Track::WAVE);
    cmbType->addItem(*addOutputIcon, tr("Output"), Track::AUDIO_OUTPUT);
    cmbType->addItem(*addInputIcon, tr("Input"), Track::AUDIO_INPUT);

    cmbInChannel->addItem(tr("All"), m_allChannelBit);
    for(int i = 0; i < 16; ++i)
    {
        cmbInChannel->addItem(QString(tr("Chan ")).append(QString::number(i+1)), 1 << i);
    }
    cmbInChannel->setCurrentIndex(1);

    //cmbOutChannel->addItem(tr("All"), m_allChannelBit);
    for(int i = 0; i < 16; ++i)
    {
        cmbOutChannel->addItem(QString(tr("Chan ")).append(QString::number(i+1)), i);
    }
    cmbOutChannel->setCurrentIndex(0);

    int row = cmbType->findData(m_insertType);
    cmbType->setCurrentIndex(row);

    connect(chkInput, SIGNAL(toggled(bool)), this, SLOT(updateInputSelected(bool)));
    connect(chkOutput, SIGNAL(toggled(bool)), this, SLOT(updateOutputSelected(bool)));
    connect(cmbType, SIGNAL(currentIndexChanged(int)), this, SLOT(trackTypeChanged(int)));
    //connect(cmbInstrument, SIGNAL(currentIndexChanged(int)), this, SLOT(updateInstrument(int)));
    connect(cmbInstrument, SIGNAL(activated(int)), this, SLOT(updateInstrument(int)));
    connect(btnAdd, SIGNAL(clicked()), this, SLOT(addTrack()));
    connect(txtName, SIGNAL(textEdited(QString)), this, SLOT(trackNameEdited()));
    connect(btnCancel, SIGNAL(clicked()), this, SLOT(cancelSelected()));
    connect(btnMyInput, SIGNAL(clicked()), this, SLOT(showInputSettings()));
    txtName->setFocus(Qt::OtherFocusReason);
}

//Add button slot
void CreateTrackDialog::addTrack()/*{{{*/
{
    if(txtName->text().isEmpty())
        return;

    int inputIndex = cmbInput->currentIndex();
    int outputIndex = cmbOutput->currentIndex();
    int instrumentIndex = cmbInstrument->currentIndex();
    int monitorIndex = cmbMonitor->currentIndex();
    int inChanIndex = cmbInChannel->currentIndex();
    int outChanIndex = cmbOutChannel->currentIndex();
    bool valid = true;

    m_vtrack->name = txtName->text();
    m_vtrack->type = m_insertType;
    Track::TrackType type = (Track::TrackType)m_insertType;
    switch(type)
    {
    case Track::MIDI:
    {
        int outChan = cmbOutChannel->itemData(outChanIndex).toInt();
        QString instrumentName = cmbInstrument->itemData(instrumentIndex, CTDInstrumentNameRole).toString();
        QString selectedInput, selectedInput2;

        //Process Input connections
        if(inputIndex >= 0 && chkInput->isChecked())/*{{{*/
        {
            m_vtrack->useInput = true;
            int chanbit = cmbInChannel->itemData(inChanIndex).toInt();
            if(inputIndex == 0)
            {
                m_vtrack->useGlobalInputs = true;
                m_vtrack->inputChannel = chanbit;
            }
            else
            {
                QString devname = cmbInput->itemText(inputIndex);
                int inDevType = cmbInput->itemData(inputIndex).toInt();
                if(m_currentMidiInputList.isEmpty())
                {
                    m_createMidiInputDevice = true;
                }
                else
                {
                    m_createMidiInputDevice = !(m_currentMidiInputList.contains(inputIndex) && m_currentMidiInputList.value(inputIndex) == devname);
                }
                m_vtrack->createMidiInputDevice = m_createMidiInputDevice;
                m_vtrack->inputConfig = qMakePair(inDevType, devname);
                m_vtrack->inputChannel = chanbit;
            }
        }/*}}}*/

        {
            if(outputIndex >= 0 && chkOutput->isChecked())
            {
                m_vtrack->useOutput = true;
                QString devname = cmbOutput->itemText(outputIndex);
                int devtype = cmbOutput->itemData(outputIndex).toInt();
                if(m_currentMidiOutputList.isEmpty())
                {
                    m_createMidiOutputDevice = true;
                }
                else
                {
                    m_createMidiOutputDevice = !(m_currentMidiOutputList.contains(outputIndex) && m_currentMidiOutputList.value(outputIndex) == devname);
                }
                m_vtrack->createMidiOutputDevice = m_createMidiOutputDevice;
                m_vtrack->outputConfig = qMakePair(devtype, devname);
                m_vtrack->outputChannel = outChan;
                if(m_createMidiOutputDevice)
                {
                    //QString instrumentName = cmbInstrument->itemData(instrumentIndex, CTDInstrumentNameRole).toString();
                    m_vtrack->instrumentName = instrumentName;
                }
            }

            if(monitorIndex >= 0 && midiBox->isChecked())
            {
                if (selectedInput.isEmpty())
                    selectedInput = cmbMonitor->itemText(monitorIndex);
                if (selectedInput2.isEmpty())
                    selectedInput2 = selectedInput;
            }
        }
    }
        break;
    case Track::WAVE:
    {
        if(inputIndex >= 0 && chkInput->isChecked())
        {
            QString selectedInput = cmbInput->itemText(inputIndex);
            int addNewRoute = cmbInput->itemData(inputIndex).toInt();
            m_vtrack->useInput = true;
            m_vtrack->inputConfig = qMakePair(addNewRoute, selectedInput);
        }
        if(outputIndex >= 0 && chkOutput->isChecked())
        {
            //Route to the Output or Buss
            QString selectedOutput = cmbOutput->itemText(outputIndex);
            m_vtrack->useOutput = true;
            m_vtrack->outputConfig = qMakePair(0, selectedOutput);
        }
    }
        break;
    case Track::AUDIO_OUTPUT:
    {
        if(inputIndex >= 0 && chkInput->isChecked())
        {
            QString selectedInput = cmbInput->itemText(inputIndex);
            m_vtrack->useInput = true;
            m_vtrack->inputConfig = qMakePair(0, selectedInput);
        }

        if(outputIndex >= 0 && chkOutput->isChecked())
        {
            QString jackPlayback("system:playback");
            QString selectedOutput = cmbOutput->itemText(outputIndex);
            m_vtrack->useOutput = true;
            m_vtrack->outputConfig = qMakePair(0, selectedOutput);
        }
    }
        break;
    case Track::AUDIO_INPUT:
    {
        if(inputIndex >= 0 && chkInput->isChecked())
        {
            QString selectedInput = cmbInput->itemText(inputIndex);
            m_vtrack->useInput = true;
            m_vtrack->inputConfig = qMakePair(0, selectedInput);
        }
        if(outputIndex >= 0 && chkOutput->isChecked())
        {
            QString selectedOutput = cmbOutput->itemText(outputIndex);
            m_vtrack->useOutput = true;
            m_vtrack->outputConfig = qMakePair(0, selectedOutput);
        }
    }
        break;
    default:
        valid = false;
    }
    if(valid)
    {
        if(m_templateMode)
        {
            //hide();
            done(1);
        }
        else
        {
            connect(trackManager, SIGNAL(trackAdded(qint64)), this, SIGNAL(trackAdded(qint64)));
            if(trackManager->addTrack(m_vtrack, m_insertPosition))
            {
                if(debugMsg)
                    qDebug("CreateTrackDialog::addTrack: Sucessfully added track");
                done(1);
            }
            else
            {
                QMessageBox::critical(this, tr("Create Track Failed"), tr("Unknown error occurred.\nFailed to add new track."));
                done(0);
            }
        }
    }
}/*}}}*/

void CreateTrackDialog::lockType(bool lock)
{
    cmbType->setEnabled(!lock);
}

void CreateTrackDialog::cancelSelected()
{
    reject();
}

void CreateTrackDialog::showInputSettings()
{
    GlobalSettingsConfig* genSetConfig = new GlobalSettingsConfig(this);
    genSetConfig->setCurrentTab(2);
    genSetConfig->show();
}

void CreateTrackDialog::updateInstrument(int index)/*{{{*/
{
    QString instrumentName = cmbInstrument->itemData(index, CTDInstrumentNameRole).toString();
    QString trackName = txtName->text();
    if(trackName.isEmpty())
    {
        trackName = Track::getValidName(instrumentName);
        txtName->setText(trackName);
        trackNameEdited();
    }
    //if(btnAdd->isEnabled())
    //{
    chkInput->setEnabled(true);
    cmbInput->setEnabled(true);
    cmbInChannel->setEnabled(true);
    chkOutput->setEnabled(true);
    cmbOutput->setEnabled(true);
    cmbOutChannel->setEnabled(true);
    midiBox->setChecked(true);
    midiBox->setEnabled(true);

    updateVisibleElements();

}/*}}}*/

//Input raw slot
void CreateTrackDialog::updateInputSelected(bool raw)/*{{{*/
{
    cmbInput->setEnabled(raw);
    cmbInChannel->setEnabled(raw);
}/*}}}*/

//Output raw slot
void CreateTrackDialog::updateOutputSelected(bool raw)/*{{{*/
{
    cmbOutput->setEnabled(raw);
    cmbOutChannel->setEnabled(raw);
}/*}}}*/

//Track type combo slot
void CreateTrackDialog::trackTypeChanged(int type)/*{{{*/
{
    m_insertType = cmbType->itemData(type).toInt();
    updateVisibleElements();
    populateInputList();
    populateOutputList();
    populateInstrumentList();
    populateMonitorList();
}/*}}}*/

void CreateTrackDialog::trackNameEdited()
{
    bool enabled = !txtName->text().isEmpty();
    btnAdd->setEnabled(enabled);
}

//Populate input combo based on type
void CreateTrackDialog::populateInputList()/*{{{*/
{
    while(cmbInput->count())
        cmbInput->removeItem(cmbInput->count()-1);
    Track::TrackType type = (Track::TrackType)m_insertType;
    switch(type)
    {
    case Track::MIDI:
    {
        m_currentMidiInputList.clear();
        m_currentInput.clear();
        cmbInput->addItem(tr("My Inputs"), 1025);
        for (int i = 0; i < MIDI_PORTS; ++i)
        {
            MidiPort* mp = &midiPorts[i];
            MidiDevice* md = mp->device();

            if (!md)
                continue;

            if ((md->openFlags() & 2))
            {
                QString mdname(md->name());
                m_currentInput.append(mdname);
                //if(md->deviceType() == MidiDevice::ALSA_MIDI)
                //{
                //Put it in the alsa list
                //}
                mdname = QString("(OOStudio) ").append(mdname);
                cmbInput->addItem(mdname, i);
                m_currentMidiInputList.insert(cmbInput->count()-1, mdname);
            }
        }
        populateNewInputList();

        if (!cmbInput->count())
        {
            chkInput->setChecked(false);
            chkInput->setEnabled(false);
        }
    }
        break;
    case Track::WAVE:
    {
        //The track now has its own input on creation so dont bother showing other input
        //to connect to it
        /*for(iTrack it = song->inputs()->begin(); it != song->inputs()->end(); ++it)
            {
                cmbInput->addItem((*it)->name(), 0);
            }*/
        importOutputs();
        if (!cmbInput->count())
        {//TODO: Not sure what we could do here except notify the user
            chkInput->setChecked(false);
            chkInput->setEnabled(false);
        }
    }
        break;
    case Track::AUDIO_OUTPUT:
    {
        for(iTrack t = song->tracks()->begin(); t != song->tracks()->end(); ++t)
        {
            if((*t)->isMidiTrack() || (*t)->type() == Track::AUDIO_OUTPUT)
                continue;
            AudioTrack* track = (AudioTrack*) (*t);
            Route r(track, -1);
            cmbInput->addItem(r.name());
        }

        if (!cmbInput->count())
        {//TODO: Not sure what we could do here except notify the user
            chkInput->setChecked(false);
            chkInput->setEnabled(false);
        }
    }
        break;
    case Track::AUDIO_INPUT:
    {
        importOutputs();
        if (!cmbInput->count())
        {//TODO: Not sure what we could do here except notify the user
            chkInput->setChecked(false);
            chkInput->setEnabled(false);
        }
    }
        break;
    }
}/*}}}*/

void CreateTrackDialog::populateOutputList()/*{{{*/
{
    while(cmbOutput->count())
        cmbOutput->removeItem(cmbOutput->count()-1);
    Track::TrackType type = (Track::TrackType)m_insertType;
    switch(type)
    {
    case Track::MIDI:
    {
        m_currentMidiOutputList.clear();
        m_currentOutput.clear();
        for (int i = 0; i < MIDI_PORTS; ++i)
        {
            MidiPort* mp = &midiPorts[i];
            MidiDevice* md = mp->device();

            if (!md)
                continue;

            if((md->openFlags() & 1))
            {
                QString mdname(md->name());
                m_currentOutput.append(mdname);
                //if(md->deviceType() == MidiDevice::ALSA_MIDI)
                //{
                //}
                mdname = QString("(OOStudio) ").append(mdname);
                cmbOutput->addItem(mdname, i);
                m_currentMidiOutputList.insert(cmbOutput->count()-1, mdname);
            }
        }

        populateNewOutputList();
        if (!cmbOutput->count())
        {
            chkOutput->setChecked(false);
            chkOutput->setEnabled(false);
        }
    }
        break;
    case Track::WAVE:
    {
        for(iTrack t = song->tracks()->begin(); t != song->tracks()->end(); ++t)
        {
            if((*t)->isMidiTrack())
                continue;
            AudioTrack* track = (AudioTrack*) (*t);
            if(track->type() == Track::AUDIO_OUTPUT)
            {
                cmbOutput->addItem(Route(track, -1).name());
            }
        }
        if (!cmbOutput->count())
        {
            chkOutput->setChecked(false);
            chkOutput->setEnabled(false);
        }
    }
        break;
    case Track::AUDIO_OUTPUT:
    {
        importInputs();
        if (!cmbOutput->count())
        {
            chkOutput->setChecked(false);
            chkOutput->setEnabled(false);
        }
    }
        break;
    case Track::AUDIO_INPUT:
    {
        for(iTrack t = song->tracks()->begin(); t != song->tracks()->end(); ++t)
        {
            if((*t)->isMidiTrack())
                continue;
            AudioTrack* track = (AudioTrack*) (*t);
            switch(track->type())
            {
            case Track::AUDIO_OUTPUT:
            case Track::WAVE:
                cmbOutput->addItem(Route(track, -1).name());
                break;
            default:
                break;
            }
        }
        if (!cmbOutput->count())
        {
            chkOutput->setChecked(false);
            chkOutput->setEnabled(false);
        }
    }
        break;
    }
}/*}}}*/

void CreateTrackDialog::importOutputs()/*{{{*/
{
    if (checkAudioDevice())
    {
        std::list<QString> sl = audioDevice->outputPorts();
        for (std::list<QString>::iterator i = sl.begin(); i != sl.end(); ++i) {
            cmbInput->addItem(*i, 1);
        }
    }
}/*}}}*/

void CreateTrackDialog::importInputs()/*{{{*/
{
    if (checkAudioDevice())
    {
        std::list<QString> sl = audioDevice->inputPorts();
        for (std::list<QString>::iterator i = sl.begin(); i != sl.end(); ++i) {
            cmbOutput->addItem(*i, 1);
        }
    }
}/*}}}*/

void CreateTrackDialog::populateMonitorList()/*{{{*/
{
    while(cmbMonitor->count())
        cmbMonitor->removeItem(cmbMonitor->count()-1);
    if (checkAudioDevice())
    {
        std::list<QString> sl = audioDevice->outputPorts();
        for (std::list<QString>::iterator i = sl.begin(); i != sl.end(); ++i) {
            cmbMonitor->addItem(*i, 1);
        }
    }
}/*}}}*/

void CreateTrackDialog::populateNewInputList()/*{{{*/
{
    for (iMidiDevice i = midiDevices.begin(); i != midiDevices.end(); ++i)
    {
        if ((*i)->deviceType() == MidiDevice::ALSA_MIDI)
        {
            if ((*i)->rwFlags() & 0x2)
            {
                //Dont add any ALSA ports that are already configured
                //An alsa device can only be connected to 1 OOM MidiPort
                if(m_currentInput.isEmpty() || !m_currentInput.contains((*i)->name()))
                    cmbInput->addItem((*i)->name(), MidiDevice::ALSA_MIDI);
            }
        }
    }
    if(audioDevice->deviceType() != AudioDevice::JACK_AUDIO)
        return;
    std::list<QString> sl = audioDevice->outputPorts(true, m_showJackAliases);
    for (std::list<QString>::iterator ip = sl.begin(); ip != sl.end(); ++ip)
    {
        if(m_currentInput.isEmpty() || !m_currentInput.contains(*ip))
            cmbInput->addItem(*ip, MidiDevice::JACK_MIDI);
    }
}/*}}}*/

void CreateTrackDialog::populateNewOutputList()/*{{{*/
{
    for (iMidiDevice i = midiDevices.begin(); i != midiDevices.end(); ++i)/*{{{*/
    {
        if ((*i)->deviceType() == MidiDevice::ALSA_MIDI)
        {
            if ((*i)->rwFlags() & 0x1)
            {
                //Dont add any ALSA ports that are already configured
                //An alsa device can only be connected to 1 OOM MidiPort
                if(m_currentOutput.isEmpty() || !m_currentOutput.contains((*i)->name()))
                    cmbOutput->addItem((*i)->name(), MidiDevice::ALSA_MIDI);
            }
        }
    }/*}}}*/
    if(audioDevice->deviceType() != AudioDevice::JACK_AUDIO)
        return;
    std::list<QString> sl = audioDevice->inputPorts(true, m_showJackAliases);
    for (std::list<QString>::iterator ip = sl.begin(); ip != sl.end(); ++ip)
    {
        if(m_currentOutput.isEmpty() || !m_currentOutput.contains(*ip))
            cmbOutput->addItem(*ip, MidiDevice::JACK_MIDI);
    }
}/*}}}*/

void CreateTrackDialog::populateInstrumentList()/*{{{*/
{
    cmbInstrument->clear();

    if (m_insertType == Track::MIDI)
    {
        // add GM first, then LS, then SYNTH
        for (iMidiInstrument i = midiInstruments.begin(); i != midiInstruments.end(); ++i)
        {
            // XXX another resource thing
            //if((*i)->isOOMInstrument() == false)
            {
                cmbInstrument->addItem(QString("(GM) ").append((*i)->iname()));
                cmbInstrument->setItemData(cmbInstrument->count()-1, (*i)->iname(), CTDInstrumentNameRole);
            }
        }

        //Default to the GM instrument
        int gm = cmbInstrument->findText("(GM) GM");
        if (gm >= 0)
            cmbInstrument->setCurrentIndex(gm);
    }
}/*}}}*/

void CreateTrackDialog::updateVisibleElements()/*{{{*/
{
    chkInput->setEnabled(true);
    chkOutput->setEnabled(true);
    chkInput->setChecked(true);
    chkOutput->setChecked(true);
    cmbMonitor->setEnabled(true);
    trackNameEdited();

    Track::TrackType type = (Track::TrackType)m_insertType;

    if (type == Track::MIDI)
    {
        chkInput->setText("MIDI Input");
        chkOutput->setText("MIDI Output");
    }
    else
    {
        chkInput->setText("Audio Input");
        chkOutput->setText("Audio Output");
    }

    switch (type)
    {
    case Track::MIDI:
    {
        cmbInChannel->setVisible(true);
        cmbOutChannel->setVisible(true);
        lblInstrument->setVisible(true);
        cmbInstrument->setVisible(true);
        cmbInstrument->setEnabled(true);
        cmbMonitor->setVisible(true);
        cmbInput->setVisible(true);
        chkInput->setVisible(true);
        cmbOutput->setVisible(true);
        chkOutput->setVisible(true);
        midiBoxFrame->setVisible(true);
        midiBoxLabelFrame->setVisible(true);
        trackNameEdited();

        //m_height = 290;
        m_height = 400;
        m_width = width();
    }
        break;
    case Track::AUDIO_OUTPUT:
    {
        cmbInChannel->setVisible(false);
        cmbOutChannel->setVisible(false);
        lblInstrument->setVisible(false);
        cmbInstrument->setVisible(false);
        cmbMonitor->setVisible(false);
        midiBoxFrame->setVisible(false);
        midiBoxLabelFrame->setVisible(false);

        cmbInput->setVisible(true);
        chkInput->setVisible(true);
        cmbOutput->setVisible(true);
        chkOutput->setVisible(true);

        //m_height = 160;
        m_height = 260;
        m_width = width();
    }
        break;
    case Track::AUDIO_INPUT:
    {
        cmbInChannel->setVisible(false);
        cmbOutChannel->setVisible(false);
        lblInstrument->setVisible(false);
        cmbInstrument->setVisible(false);
        cmbMonitor->setVisible(false);
        midiBoxFrame->setVisible(false);
        midiBoxLabelFrame->setVisible(false);

        cmbInput->setVisible(true);
        chkInput->setVisible(true);
        cmbOutput->setVisible(true);
        chkOutput->setVisible(true);

        //m_height = 160;
        m_height = 260;
        m_width = width();
    }
        break;
    case Track::WAVE:
    {
        cmbInChannel->setVisible(false);
        cmbOutChannel->setVisible(false);
        lblInstrument->setVisible(false);
        cmbInstrument->setVisible(false);
        cmbMonitor->setVisible(false);
        midiBoxFrame->setVisible(false);
        midiBoxLabelFrame->setVisible(false);

        cmbInput->setVisible(true);
        chkInput->setVisible(true);
        cmbOutput->setVisible(true);
        chkOutput->setVisible(true);
        chkInput->setChecked(false);

        //m_height = 160;
        m_height = 260;
        m_width = width();
    }
        break;
    default:
        break;
    }
    setFixedHeight(m_height);
    updateGeometry();
}/*}}}*/

void CreateTrackDialog::showEvent(QShowEvent*)
{
    if(debugMsg)
        qDebug("CreateTrackDialog::showEvent: trackType: %i, position: %i", m_insertType, m_insertPosition);
    updateVisibleElements();
    populateInputList();
    populateOutputList();
    populateInstrumentList();
    populateMonitorList();
}

