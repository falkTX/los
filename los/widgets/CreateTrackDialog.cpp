//===========================================================
//  LOS
//  Libre Octave Studio
//  (C) Copyright 2011 Andrew Williams & Christopher Cherrett
//===========================================================

#include "CreateTrackDialog.h"
#include "NameValidator.h"

#include "audio.h"
#include "driver/audiodev.h"
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

static const int s_allChannelBit = (1 << kMaxMidiChannels) - 1;

CreateTrackDialog::CreateTrackDialog(int pos, QWidget* parent)
    : QDialog(parent),
      m_insertPosition(pos)
{
    initDefaults();
    m_vtrack = new VirtualTrack;
}

CreateTrackDialog::CreateTrackDialog(VirtualTrack** vt, int pos, QWidget* parent)
    : QDialog(parent),
      m_insertPosition(pos)
{
    initDefaults();
    m_vtrack = new VirtualTrack;
    *vt = m_vtrack;
}

void CreateTrackDialog::initDefaults()
{
    setupUi(this);

    m_width  = 450;
    m_height = 290;

    m_createMidiInputDevice = false;
    m_createMidiOutputDevice = false;

    txtName->setValidator(new NameValidator(this));

    cmbInChannel->addItem(tr("All"), s_allChannelBit);
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

    connect(chkInput, SIGNAL(toggled(bool)), this, SLOT(updateInputSelected(bool)));
    connect(chkOutput, SIGNAL(toggled(bool)), this, SLOT(updateOutputSelected(bool)));
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
    int inChanIndex = cmbInChannel->currentIndex();
    int outChanIndex = cmbOutChannel->currentIndex();
    bool valid = true;

    m_vtrack->name = txtName->text();

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
        }
    }
    if(valid)
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
}/*}}}*/

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

    chkInput->setEnabled(true);
    cmbInput->setEnabled(true);
    cmbInChannel->setEnabled(true);
    chkOutput->setEnabled(true);
    cmbOutput->setEnabled(true);
    cmbOutChannel->setEnabled(true);

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

    {
        m_currentMidiInputList.clear();
        m_currentInput.clear();

        cmbInput->addItem(tr("My MIDI Inputs"), 1025);

        for (int i = 0; i < kMaxMidiPorts; ++i)
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
                mdname = QString("(LOS) ").append(mdname);
                cmbInput->addItem(mdname, i);
                m_currentMidiInputList.insert(cmbInput->count()-1, mdname);
            }
        }

        populateMidiInputList();

        if (!cmbInput->count())
        {
            chkInput->setChecked(false);
            chkInput->setEnabled(false);
        }
    }
}/*}}}*/

void CreateTrackDialog::populateOutputList()/*{{{*/
{
    while(cmbOutput->count())
        cmbOutput->removeItem(cmbOutput->count()-1);

    {
        m_currentMidiOutputList.clear();
        m_currentOutput.clear();

        for (int i = 0; i < kMaxMidiPorts; ++i)
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
                mdname = QString("(LOS) ").append(mdname);
                cmbOutput->addItem(mdname, i);
                m_currentMidiOutputList.insert(cmbOutput->count()-1, mdname);
            }
        }

        populateMidiOutputList();

        if (!cmbOutput->count())
        {
            chkOutput->setChecked(false);
            chkOutput->setEnabled(false);
        }
    }
}/*}}}*/

void CreateTrackDialog::populateMidiInputList()/*{{{*/
{
    for (iMidiDevice i = midiDevices.begin(); i != midiDevices.end(); ++i)
    {
        if ((*i)->deviceType() == MidiDevice::ALSA_MIDI)
        {
            if ((*i)->rwFlags() & 0x2)
            {
                //Dont add any ALSA ports that are already configured
                //An alsa device can only be connected to 1 LOS MidiPort
                if(m_currentInput.isEmpty() || !m_currentInput.contains((*i)->name()))
                    cmbInput->addItem((*i)->name(), MidiDevice::ALSA_MIDI);
            }
        }
    }
    if(audioDevice->deviceType() != AudioDevice::JACK_AUDIO)
        return;
    std::list<QString> sl = audioDevice->outputPorts(true, -1);
    for (std::list<QString>::iterator ip = sl.begin(); ip != sl.end(); ++ip)
    {
        if(m_currentInput.isEmpty() || !m_currentInput.contains(*ip))
            cmbInput->addItem(*ip, MidiDevice::JACK_MIDI);
    }
}/*}}}*/

void CreateTrackDialog::populateMidiOutputList()/*{{{*/
{
    for (iMidiDevice i = midiDevices.begin(); i != midiDevices.end(); ++i)/*{{{*/
    {
        if ((*i)->deviceType() == MidiDevice::ALSA_MIDI)
        {
            if ((*i)->rwFlags() & 0x1)
            {
                //Dont add any ALSA ports that are already configured
                //An alsa device can only be connected to 1 LOS MidiPort
                if(m_currentOutput.isEmpty() || !m_currentOutput.contains((*i)->name()))
                    cmbOutput->addItem((*i)->name(), MidiDevice::ALSA_MIDI);
            }
        }
    }/*}}}*/
    if(audioDevice->deviceType() != AudioDevice::JACK_AUDIO)
        return;
    std::list<QString> sl = audioDevice->inputPorts(true, -1);
    for (std::list<QString>::iterator ip = sl.begin(); ip != sl.end(); ++ip)
    {
        if(m_currentOutput.isEmpty() || !m_currentOutput.contains(*ip))
            cmbOutput->addItem(*ip, MidiDevice::JACK_MIDI);
    }
}/*}}}*/

void CreateTrackDialog::populateInstrumentList()/*{{{*/
{
    cmbInstrument->clear();

    {
        // add GM first, then LS, then SYNTH
        for (iMidiInstrument i = midiInstruments.begin(); i != midiInstruments.end(); ++i)
        {
            // XXX another resource thing
            //if((*i)->isLOSInstrument() == false)
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

    trackNameEdited();

    {
        chkInput->setText("MIDI Input");
        chkOutput->setText("MIDI Output");
    }

    {
        cmbInChannel->setVisible(true);
        cmbOutChannel->setVisible(true);
        lblInstrument->setVisible(true);
        cmbInstrument->setVisible(true);
        cmbInstrument->setEnabled(true);
        cmbInput->setVisible(true);
        chkInput->setVisible(true);
        cmbOutput->setVisible(true);
        chkOutput->setVisible(true);

        trackNameEdited();

        m_height = 300;
        m_width = width();
    }
    setFixedHeight(m_height);
    updateGeometry();
}/*}}}*/

void CreateTrackDialog::showEvent(QShowEvent*)
{
    updateVisibleElements();
    populateInputList();
    populateOutputList();
    populateInstrumentList();
}

