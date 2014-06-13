//=========================================================
//  LOS
//  Libre Octave Studio
//  $Id: $
//
//  (C) Copyright 2011 Andrew Williams and Christopher Cherrett
//=========================================================

#include "song.h"
#include "globals.h"
#include "config.h"
#include "icons.h"
#include "gconfig.h"
#include "midiport.h"
#include "instruments/minstrument.h"
#include "mididev.h"
#include "utils.h"
#include "audio.h"
#include "midi.h"
#include "midictrl.h"
#include "midiassign.h"
#include "tablespinner.h"
#include "midiportdelegate.h"
#include "midipresetdelegate.h"
#include "ccdelegate.h"
#include "ccinfo.h"
#include "midimonitor.h"
#include "confmport.h"
#include "driver/audiodev.h"
#include "traverso_shared/TConfig.h"

#include <QStringList>
#include <QList>
#include <QMessageBox>

MidiAssignDialog::MidiAssignDialog(QWidget* parent):QDialog(parent)
{
    setupUi(this);
    m_lasttype = 0;
    m_selected = 0;
    m_selectport = 0;

    midiPortConfig = new MPConfig(this);
    //m_tabpanel->insertTab(0, audioPortConfig, tr("Audio Routing Manager"));
    m_tabpanel->insertTab(0, midiPortConfig, tr("Midi Port Manager"));
    m_btnReset = m_buttonBox->button(QDialogButtonBox::Reset);

    m_btnDelete->setIcon(*garbageIconSet3);

    m_btnAdd->setIcon(*plusIconSet3);

    m_assignlabels = (QStringList() << "En" << "Track" << "Midi Port" << "Chan" << "Preset" );
    m_cclabels = (QStringList() << "Sel" << "Controller");
    m_mplabels = (QStringList() << "Midi Port");
    m_presetlabels = (QStringList() << "Sel" << "Id" << "Sysex Text");

    m_allowed << CTRL_VOLUME << CTRL_PANPOT << CTRL_REVERB_SEND << CTRL_CHORUS_SEND << CTRL_VARIATION_SEND << CTRL_RECORD << CTRL_MUTE << CTRL_SOLO ;

    m_model = new QStandardItemModel(0, 5, this);
    tableView->setModel(m_model);
    m_selmodel = new QItemSelectionModel(m_model);
    tableView->setSelectionModel(m_selmodel);

    m_ccmodel = new QStandardItemModel(0, 2, this);
    m_ccEdit->setModel(m_ccmodel);
    m_ccEdit->setSortingEnabled(true);

    _trackTypes = (QStringList() << "All Types" << "Outputs" << "Inputs" << "Auxs" << "Busses" << "Midi Tracks" << "Soft Synth" << "Audio Tracks");
    cmbType->addItems(_trackTypes);
    connect(cmbType, SIGNAL(currentIndexChanged(int)), SLOT(cmbTypeSelected(int)));
    cmbType->setCurrentIndex(0);

    //TableSpinnerDelegate* tdelegate = new TableSpinnerDelegate(this, -1);
    TableSpinnerDelegate* chandelegate = new TableSpinnerDelegate(this, 1, 16);
    MidiPortDelegate* mpdelegate = new MidiPortDelegate(this);
    MidiPresetDelegate* presetdelegate = new MidiPresetDelegate(this);
    CCInfoDelegate *infodelegate = new CCInfoDelegate(this);
    tableView->setItemDelegateForColumn(2, mpdelegate);
    tableView->setItemDelegateForColumn(3, chandelegate);
    tableView->setItemDelegateForColumn(4, presetdelegate);
    m_ccmodel->setSortRole(CCSortRole);

    m_ccEdit->setItemDelegateForColumn(1, infodelegate);
    m_cmbControl->addItem(midiControlToString(CTRL_RECORD), CTRL_RECORD);
    m_cmbControl->addItem(midiControlToString(CTRL_MUTE), CTRL_MUTE);
    m_cmbControl->addItem(midiControlToString(CTRL_SOLO), CTRL_SOLO);

    for(int i = 0; i < 128; ++i)
    {
        QString ctl(QString::number(i)+": ");
        m_cmbControl->addItem(ctl.append(midiCtrlName(i)), i);
    }

    //populateMidiPorts();

    connect(m_btnReset, SIGNAL(clicked(bool)), SLOT(btnResetClicked()));
    connect(m_model, SIGNAL(itemChanged(QStandardItem*)), SLOT(itemChanged(QStandardItem*)));
    connect(m_selmodel, SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), SLOT(itemSelected(const QItemSelection&, const QItemSelection&)));
    connect(m_btnAdd, SIGNAL(clicked()), SLOT(btnAddController()));
    connect(m_btnDelete, SIGNAL(clicked()), SLOT(btnDeleteController()));
    connect(m_btnDefault, SIGNAL(clicked()), SLOT(btnUpdateDefault()));
    connect(m_tabpanel, SIGNAL(currentChanged(int)), SLOT(currentTabChanged(int)));

    cmbTypeSelected(m_lasttype);
    switchTabs(0);
}

MidiAssignDialog::~MidiAssignDialog()
{
    tconfig().set_property("ConnectionsManager", "size", size());
    tconfig().set_property("ConnectionsManager", "pos", pos());
    // Save the new global settings to the configuration file
    tconfig().save();
}

void MidiAssignDialog::itemChanged(QStandardItem* item)/*{{{*/
{
    //printf("itemChanged\n");
    if(item)
    {
        int row = item->row();
        int col = item->column();
        QStandardItem* trk = m_model->item(row, 1);
        MidiTrack* track = song->findTrack(trk->text());
        if(track)
        {
            MidiAssignData* data = track->midiAssign();
            switch(col)
            {
                case 0: //Enabled
                    data->enabled = (item->checkState() == Qt::Checked);
                break;
                case 1: //Track not editable
                break;
                case 2: //Port
                {
                    int p = data->port;
                    data->port = item->data(MidiPortRole).toInt();
                    QHashIterator<int, CCInfo*> iter(data->midimap);
                    while(iter.hasNext())
                    {
                        iter.next();
                        CCInfo* info = iter.value();
                        info->setPort(data->port);
                    }
                    midiMonitor->msgModifyTrackPort(track, p);
                }
                break;
                case 3: //Channel
                    data->channel = item->data(Qt::EditRole).toInt()-1;
                break;
                case 4: //Preset
                    data->preset = item->data(MidiPresetRole).toInt();
                break;
            }
        }
    }
}/*}}}*/

//Update the ccEdit table
void MidiAssignDialog::itemSelected(const QItemSelection& isel, const QItemSelection&) /*{{{*/
{
    //printf("Selection changed\n");
    m_ccmodel->clear();
    QModelIndexList list = isel.indexes();
    if(list.size() > 0)
    {
        QModelIndex index = list.at(0);
        int row = index.row();
        QStandardItem* item = m_model->item(row, 1);
        if(item)
        {
            MidiTrack* track = song->findTrack(item->text());
            if(track)
            {
                m_trackname->setText(track->name());
                m_selected = track;
                MidiAssignData* data = track->midiAssign();
                if(data && !data->midimap.isEmpty())
                {
                    QHashIterator<int, CCInfo*> iter(data->midimap);
                    while(iter.hasNext())
                    {
                        iter.next();
                        CCInfo* info = iter.value();
                        QList<QStandardItem*> rowData;
                        QStandardItem* chk = new QStandardItem(data->enabled);
                        chk->setCheckable(true);
                        chk->setEditable(false);
                        rowData.append(chk);
                        QStandardItem* control = new QStandardItem(track->name());
                        control->setEditable(true);
                        control->setData(track->name(), TrackRole);
                        control->setData(info->port(), PortRole);
                        control->setData(info->channel(), ChannelRole);
                        control->setData(info->controller(), ControlRole);
                        control->setData(info->assignedControl(), CCRole);
                        control->setData(midiControlSortIndex(info->controller()), CCSortRole);
                        QString str;
                        str.append("( ").append(midiControlToString(info->controller())).append(" )");
                        if(info->assignedControl() >= 0)
                            str.append(" Assigned to CC: ").append(QString::number(info->assignedControl())).append(" on Chan: ").append(QString::number(info->channel()+1));
                        control->setData(str, Qt::DisplayRole);
                        rowData.append(control);
                        m_ccmodel->appendRow(rowData);
                        m_ccEdit->setRowHeight(m_ccmodel->rowCount()-1, 50);
                    }
                }
            }
        }
    }
    updateCCTableHeader();
}/*}}}*/

void MidiAssignDialog::btnAddController()/*{{{*/
{
    if(!m_selected)
        return;
    int ctrl = m_cmbControl->itemData(m_cmbControl->currentIndex()).toInt();
    MidiAssignData *data = m_selected->midiAssign();
    if(data)
    {
        bool allowed = true;
#if 0
        if(!m_selected->isMidiTrack())
        {
            allowed = false;
            switch(ctrl)//{{{
            {
                case CTRL_VOLUME:
                case CTRL_PANPOT:
                case CTRL_MUTE:
                case CTRL_SOLO:
                    allowed = true;
                break;
                case CTRL_RECORD:
                    if(m_selected->type() == Track::WAVE)
                    {
                        allowed = true;
                    }
                break;
            }//}}}
        }
#endif
        if(!allowed)
            return;
        if(data->midimap.isEmpty() || !data->midimap.contains(ctrl))
        {
            CCInfo* info = new CCInfo(m_selected, data->port, data->channel, ctrl, -1);
            data->midimap.insert(ctrl, info);
            QList<QStandardItem*> rowData;
            QStandardItem* chk = new QStandardItem(data->enabled);
            chk->setCheckable(true);
            chk->setEditable(false);
            rowData.append(chk);
            QStandardItem* control = new QStandardItem(m_selected->name());
            control->setEditable(true);
            control->setData(m_selected->name(), TrackRole);
            control->setData(info->port(), PortRole);
            control->setData(info->channel(), ChannelRole);
            control->setData(info->controller(), ControlRole);
            control->setData(info->assignedControl(), CCRole);
            control->setData(midiControlSortIndex(info->controller()), CCSortRole);
            QString str;
            str.append("( ").append(midiControlToString(info->controller())).append(" )");
            if(info->assignedControl() >= 0)
                str.append(" Assigned to CC: ").append(QString::number(info->assignedControl())).append(" on Chan: ").append(QString::number(info->channel()));
            control->setData(str, Qt::DisplayRole);
            rowData.append(control);
            m_ccmodel->appendRow(rowData);
            m_ccEdit->setRowHeight(m_ccmodel->rowCount()-1, 50);
            song->dirty = true;
        }
    }
    updateCCTableHeader();
}/*}}}*/

void MidiAssignDialog::btnDeleteController()/*{{{*/
{
    if(!m_selected)
        return;
    MidiAssignData* data = m_selected->midiAssign();
    for(int i = 0; i < m_ccmodel->rowCount(); ++i)
    {
        QStandardItem* item = m_ccmodel->item(i, 0);
        if(item->checkState() == Qt::Checked)
        {
            QStandardItem* ctl = m_ccmodel->item(i, 1);
            int control = ctl->data(ControlRole).toInt();
            if(!data->midimap.isEmpty() && data->midimap.contains(control))
            {
                //printf("Delete clicked\n");
                CCInfo* info = data->midimap.value(control);
                midiMonitor->msgDeleteTrackController(info);
                data->midimap.remove(control);
                m_ccmodel->takeRow(i);
                song->dirty = true;
            }
        }
    }
    updateCCTableHeader();
}/*}}}*/

void MidiAssignDialog::btnUpdateDefault()/*{{{*/
{
    //printf("MidiAssignDialog::btnUpdateDefault rowCount:%d \n", m_model->rowCount());
    bool override2 = false;
    if(m_chkOverride->isChecked())
    {
        int btn = QMessageBox::question(this, tr("Midi Assign Change"), tr("You are about to override the settings of pre-assigned tracks.\nAre you sure you want to do this?"),QMessageBox::Ok|QMessageBox::Cancel);
        if(btn == QMessageBox::Ok)
            override2 = true;
        else
            return; //Dont do anything as they canceled
    }
    for(int i = 0; i < m_model->rowCount(); ++i)
    {
        QStandardItem* trk = m_model->item(i, 1);
        MidiTrack* track = song->findTrack(trk->text());
        if(track)
        {
            MidiAssignData* data = track->midiAssign();
            bool unassigned = true;
            int p = data->port;
            QHashIterator<int, CCInfo*> iter(data->midimap);
            if(!override2)
            {
                while(iter.hasNext())
                {
                    iter.next();
                    CCInfo* info = iter.value();
                    if(info->assignedControl() >= 0)
                    {
                        unassigned = false;
                        break;
                    }
                }
            }
            if(unassigned)
            {
                iter.toFront();
                data->port = m_cmbPort->currentIndex();
                while(iter.hasNext())
                {
                    iter.next();
                    CCInfo* info = iter.value();
                    info->setPort(data->port);
                }
                midiMonitor->msgModifyTrackPort(track, p);
            }
        }
    }
    //Update the view
    m_ccmodel->clear();
    m_trackname->setText("");
    cmbTypeSelected(m_lasttype);
}/*}}}*/

void MidiAssignDialog::cmbTypeSelected(int type)/*{{{*/
{
    //Perform actions to populate list below based on selected type
    m_lasttype = type;
    QString defaultname;
    defaultname.sprintf("%d:%s", 1, midiPorts[0].portname().toLatin1().constData());
    m_model->clear();
    m_ccmodel->clear();

    for (ciMidiTrack t = song->tracks()->begin(); t != song->tracks()->end(); ++t)
    {
        MidiAssignData* data = (*t)->midiAssign();
        QList<QStandardItem*> rowData;
        QStandardItem* enable = new QStandardItem(data->enabled);
        enable->setCheckable(true);
        enable->setCheckState(data->enabled ? Qt::Checked : Qt::Unchecked);
        enable->setEditable(false);
        rowData.append(enable);
        QStandardItem* trk = new QStandardItem((*t)->name());
        trk->setEditable(false);
        rowData.append(trk);
        //printf("MidiPort from Assign %d\n", data->port);
        QString pname;
        if(data->port >= 0)
            pname.sprintf("%d:%s", data->port + 1, midiPorts[data->port].portname().toLatin1().constData());
        else
            pname = defaultname;
        QStandardItem* port = new QStandardItem(pname);
        port->setData(data->port, MidiPortRole);
        port->setEditable(true);
        rowData.append(port);
        QStandardItem* chan = new QStandardItem(QString::number(data->channel+1));
        chan->setData(data->channel+1, Qt::EditRole);
        chan->setEditable(true);
        rowData.append(chan);
        QStandardItem* preset = new QStandardItem(QString::number(data->preset));
        if(!data->preset)
            preset->setData(tr("None"), Qt::DisplayRole);
        preset->setData(data->preset, MidiPresetRole);
        preset->setEditable(true);
        rowData.append(preset);
        m_model->appendRow(rowData);
    }
    updateTableHeader();
    updateCCTableHeader();
}/*}}}*/

void MidiAssignDialog::updateTableHeader()/*{{{*/
{
    tableView->setColumnWidth(0, 25);
    tableView->setColumnWidth(1, 180);
    tableView->setColumnWidth(2, 120);
    tableView->setColumnWidth(3, 60);
    tableView->setColumnWidth(4, 50);
    m_model->setHorizontalHeaderLabels(m_assignlabels);
    tableView->horizontalHeader()->setStretchLastSection(true);
}/*}}}*/

void MidiAssignDialog::updateCCTableHeader()/*{{{*/
{
    m_ccEdit->setColumnWidth(0, 30);
    m_ccEdit->setColumnWidth(1, 180);
    m_ccmodel->setHorizontalHeaderLabels(m_cclabels);
    m_ccEdit->horizontalHeader()->setStretchLastSection(true);
    m_ccEdit->horizontalHeader()->setSortIndicatorShown(false);
    m_ccEdit->sortByColumn(1, Qt::AscendingOrder);
}/*}}}*/

void MidiAssignDialog::btnResetClicked()/*{{{*/
{
    m_selectport = 0;
    m_ccmodel->clear();
    m_trackname->setText("");
    cmbTypeSelected(m_lasttype);

    //if(midiSyncConfig)
    //	midiSyncConfig->songChanged(-1);
    if(midiPortConfig)
        midiPortConfig->songChanged(-1);
}/*}}}*/

void MidiAssignDialog::currentTabChanged(int flags)/*{{{*/
{
    switch(flags)
    {
        case 0: //AudioPortConfig
        break;
        case 1: //MidiPortConfig
            midiPortConfig->songChanged(-1);
        break;
        case 2: //MidiPortPreset
        case 3: //MidiAssign
        break;
        //case 4: //MidiSync
        //	midiSyncConfig->songChanged(-1);
        //break;
    }
}/*}}}*/

void MidiAssignDialog::switchTabs(int tab)
{
    m_tabpanel->setCurrentIndex(tab);
}

void MidiAssignDialog::updateJackMaster(int val)
{
    if (audioDevice)
        audioDevice->setMaster(val == Qt::Checked);
}

//Virtuals
void MidiAssignDialog::showEvent(QShowEvent*)
{
    currentTabChanged(m_tabpanel->currentIndex());
    resize(tconfig().get_property("ConnectionsManager", "size", QSize(891, 691)).toSize());
    move(tconfig().get_property("ConnectionsManager", "pos", QPoint(0, 0)).toPoint());
}

void MidiAssignDialog::closeEvent(QCloseEvent*)
{
    tconfig().set_property("ConnectionsManager", "size", size());
    tconfig().set_property("ConnectionsManager", "pos", pos());
    // Save the new global settings to the configuration file
    tconfig().save();
}
