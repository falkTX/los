//=========================================================
//  OOStudio
//  OpenOctave Midi and Audio Editor
//  $Id: plugin.h,v 1.9.2.13 2009/12/06 01:25:21 terminator356 Exp $
//
//  (C) Copyright 2000 Werner Schweer (ws@seh.de)
//  (C) Copyright 2011 Andrew Williams and Christopher Cherrett
//=========================================================

#include <QtGui>
#include "config.h"
#include "plugindialog.h"
#include "track.h"
#include "traverso_shared/TConfig.h"

int PluginDialog::selectedPlugType = 0;
QStringList PluginDialog::sortItems = QStringList();

//---------------------------------------------------------
//   PluginDialog
//    select Plugin dialog
//---------------------------------------------------------

PluginDialog::PluginDialog(int type, QWidget* parent)
: QDialog(parent)
{
}

void PluginDialog::showEvent(QShowEvent*)
{
}

void PluginDialog::closeEvent(QCloseEvent*)
{
}

void PluginDialog::hideEvent(QHideEvent* e)
{
}

//---------------------------------------------------------
//   enableOkB
//---------------------------------------------------------

void PluginDialog::enableOkB()
{
}

//---------------------------------------------------------
//   value
//---------------------------------------------------------

PluginI* PluginDialog::value()/*{{{*/
{
	printf("plugin not found\n");
	return 0;
}/*}}}*/

//---------------------------------------------------------
//   accept
//---------------------------------------------------------

void PluginDialog::accept()/*{{{*/
{
	QDialog::accept();
}/*}}}*/

void PluginDialog::typeChanged(int index)
{
}

//---------------------------------------------------------
//    fillPlugs
//---------------------------------------------------------

void PluginDialog::fillPlugs(QAbstractButton* ab)/*{{{*/
{
}/*}}}*/

void PluginDialog::fillPlugs(int nbr)/*{{{*/
{
}/*}}}*/

void PluginDialog::fillPlugs(const QString &sortValue)/*{{{*/
{
}/*}}}*/

//---------------------------------------------------------
//   getPlugin
//---------------------------------------------------------

PluginI* PluginDialog::getPlugin(int type, QWidget* parent)/*{{{*/
{
	return 0;
}/*}}}*/

