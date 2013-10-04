//=========================================================
//  OOMidi
//  OpenOctave Midi and Audio Editor
//  $Id: plugin.h,v 1.9.2.13 2009/12/06 01:25:21 terminator356 Exp $
//
//  (C) Copyright 2000 Werner Schweer (ws@seh.de)
//  (C) Copyright 2011 Andrew Williams and Christopher Cherrett
//  (C) Copyright 2012 Filipe Coelho
//=========================================================

#include "plugingui.h"

#include <QAction>
#include <QGridLayout>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QScrollArea>
#include <QTimer>

#include <math.h>

#include "checkbox.h"
#include "doublelabel.h"
#include "slider.h"

#include "filedialog.h"
#include "globals.h"
#include "icons.h"
#include "plugin.h"
#include "song.h"

//---------------------------------------------------------
//   PluginGui
//---------------------------------------------------------

PluginGui::PluginGui(BasePlugin* p)
: QMainWindow(0),
  plugin(p)
{
}

//---------------------------------------------------------
//   PluginGui
//---------------------------------------------------------

PluginGui::~PluginGui()
{
    if (params)
        delete[] params;
}


//---------------------------------------------------------
//   heartBeat
//---------------------------------------------------------

void PluginGui::heartBeat()
{
    updateControls();
}

//---------------------------------------------------------
//   setActive
//---------------------------------------------------------

void PluginGui::setActive(bool yesno)
{
    pluginBypass->blockSignals(true);
    pluginBypass->setChecked(!yesno);
    pluginBypass->blockSignals(false);
}

//---------------------------------------------------------
//   updateValues
//---------------------------------------------------------

void PluginGui::updateValues()
{
}

//---------------------------------------------------------
//   setParameterValue
//---------------------------------------------------------

void PluginGui::setParameterValue(int index, double value)
{
}

//---------------------------------------------------------
//   updateControls
//---------------------------------------------------------

void PluginGui::updateControls()
{
}
//---------------------------------------------------------
//   load
//---------------------------------------------------------

void PluginGui::load()
{
}

//---------------------------------------------------------
//   save
//---------------------------------------------------------

void PluginGui::save()
{
}

//---------------------------------------------------------
//   reset
//---------------------------------------------------------

void PluginGui::reset()
{
}

//---------------------------------------------------------
//   bypassToggled
//---------------------------------------------------------

void PluginGui::bypassToggled(bool val)
{
}

//---------------------------------------------------------
//   sliderChanged
//---------------------------------------------------------

void PluginGui::sliderChanged(double val, int param)
{
}

//---------------------------------------------------------
//   labelChanged
//---------------------------------------------------------

void PluginGui::labelChanged(double val, int param)/*{{{*/
{
}/*}}}*/

//---------------------------------------------------------
//   ctrlPressed
//---------------------------------------------------------

void PluginGui::ctrlPressed(int param)
{
}

//---------------------------------------------------------
//   ctrlReleased
//---------------------------------------------------------

void PluginGui::ctrlReleased(int param)
{
}

//---------------------------------------------------------
//   ctrlRightClicked
//---------------------------------------------------------

void PluginGui::ctrlRightClicked(const QPoint &p, int param)
{
}

//---------------------------------------------------------
//   populatePresetMenu
//---------------------------------------------------------

void PluginGui::populatePresetMenu()
{
}

void PluginGui::programSelected()
{
}
