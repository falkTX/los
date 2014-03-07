//=========================================================
//  LOS
//  Libre Octave Studio
//  $Id: $
//
//  (C) Copyright 2011 Andrew Williams and Christopher Cherrett
//=========================================================


#ifndef _OOM_MASSIGNDIALOG_
#define _OOM_MASSIGNDIALOG_

#include "ui_midiassignbase.h"
#include <QStringList>
#include <QList>

class QDialog;
class QStandardItem;
class QStandardItemModel;
class QItemSelectionModel;
class QItemSelection;
class MidiPort;
class QString;
class QShowEvent;
class QCloseEvent;
class QPushButton;
class MPConfig;
class AudioPortConfig;
class MidiTrack;

class MidiAssignDialog : public QDialog, public Ui::MidiAssignBase
{
    Q_OBJECT

    QStandardItemModel *m_model;
    QStandardItemModel *m_ccmodel;
    QItemSelectionModel *m_selmodel;

    QStringList m_assignlabels;
    QStringList m_cclabels;
    QStringList m_mplabels;
    QStringList m_presetlabels;

    QStringList _trackTypes;
    QList<int> m_allowed;
    MidiTrack* m_selected;
    MidiPort* m_selectport;
    int m_lasttype;
    QPushButton* m_btnReset;
    MPConfig *midiPortConfig;

public:
    MidiAssignDialog(QWidget* parent = 0);
    ~MidiAssignDialog();

    MPConfig* getMidiPortConfig()
    {
        return midiPortConfig;
    }

private slots:
    void btnResetClicked();
    void cmbTypeSelected(int);
    void updateTableHeader();
    void updateCCTableHeader();
    void itemChanged(QStandardItem*);
    void itemSelected(const QItemSelection&, const QItemSelection&); //Update the ccEdit table
    //Deals with the m_ccEdit table on a per track basis
    void btnAddController();
    void btnDeleteController();
    void btnUpdateDefault();
    void currentTabChanged(int);

    //midi sync transport
    void updateJackMaster(int);

public slots:
    void switchTabs(int tab = 0);

protected:
    virtual void showEvent(QShowEvent*);
    virtual void closeEvent(QCloseEvent*);
};
#endif
