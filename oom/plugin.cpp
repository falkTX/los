//=========================================================
//  OOMidi
//  OpenOctave Midi and Audio Editor
//  $Id: plugin.cpp,v 1.21.2.23 2009/12/15 22:07:12 spamatica Exp $
//
//  (C) Copyright 2000 Werner Schweer (ws@seh.de)
//=========================================================

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <cmath>
#include <math.h>

#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QDir>
#include <QFile>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <QSignalMapper>
#include <QSizePolicy>
#include <QScrollArea>
#include <QTimer>
#include <QToolBar>
#include <QToolButton>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QWhatsThis>

#include <QStandardItem>
#include <QStandardItemModel>

#include "globals.h"
#include "gconfig.h"
#include "filedialog.h"
#include "slider.h"
#include "midictrl.h"
#include "plugin.h"
#include "xml.h"
#include "icons.h"
#include "song.h"
#include "doublelabel.h"
#include "fastlog.h"
#include "checkbox.h"

#include "audio.h"
#include "audiodev.h"
#include "track.h"
#include "al/dsp.h"

#include "config.h"
#include "plugingui.h"
#include "plugindialog.h"

//---------------------------------------------------------
//   Pipeline
//---------------------------------------------------------

Pipeline::Pipeline()
: std::vector<BasePlugin*>()
{
    for (int i = 0; i < MAX_CHANNELS; ++i)
        posix_memalign((void**) (buffer + i), 16, sizeof (float) * segmentSize);
}

//---------------------------------------------------------
//   ~Pipeline
//---------------------------------------------------------

Pipeline::~Pipeline()
{
    removeAll();
    for (int i = 0; i < MAX_CHANNELS; ++i)
        ::free(buffer[i]);
}

//---------------------------------------------------------
//   insert
//    give ownership of object plugin to Pipeline
//---------------------------------------------------------

int Pipeline::addPlugin(BasePlugin* plugin, int index)
{
	int s = size();
	if(index >= s || index == -1)
	{
		push_back(plugin);
		return s;
	}
	else
	{
    	//remove(index);
    	//(*this)[index] = plugin;
    	insert(begin()+index, plugin);
		return index;
	}
}

//---------------------------------------------------------
//   remove
//---------------------------------------------------------

void Pipeline::remove(int index)
{
	int s = size();
	if(index >= s)
		return;
	if(debugMsg)
		qDebug(" Pipeline::remove(%d)", index);


	erase(begin()+index);
    //(*this)[index] = 0;
}

//---------------------------------------------------------
//   removeAll
//---------------------------------------------------------

void Pipeline::removeAll()
{
	int s = size();
    for (int i = 0; i < s; ++i)
        remove(i);
}

//---------------------------------------------------------
//   isActive
//---------------------------------------------------------

bool Pipeline::isActive(int) const
{
    return false;
}

//---------------------------------------------------------
//   setActive
//---------------------------------------------------------

void Pipeline::setActive(int, bool)
{
}

//---------------------------------------------------------
//   setChannels
//---------------------------------------------------------

void Pipeline::setChannels(int)
{
}

//---------------------------------------------------------
//   label
//---------------------------------------------------------

QString Pipeline::label(int) const
{
    return QString("");
}

//---------------------------------------------------------
//   name
//---------------------------------------------------------

QString Pipeline::name(int) const
{
    return QString("empty");
}

//---------------------------------------------------------
//   empty
//---------------------------------------------------------

bool Pipeline::empty(int idx) const
{
	int s = size();
	return idx >= s;
    //BasePlugin* p = (*this)[idx];
    //return p == 0;
}

//---------------------------------------------------------
//   move
//---------------------------------------------------------

void Pipeline::move(int, bool)
{
}

//---------------------------------------------------------
//   apply
//---------------------------------------------------------

void Pipeline::apply(int, uint32_t, float**)
{
}

//---------------------------------------------------------
//   showGui
//---------------------------------------------------------

void Pipeline::showGui(int /*idx*/, bool /*flag*/)
{
}

//---------------------------------------------------------
//   deleteGui
//---------------------------------------------------------

void Pipeline::deleteGui(int /*idx*/)
{
}

//---------------------------------------------------------
//   deleteAllGuis
//---------------------------------------------------------

void Pipeline::deleteAllGuis()
{
	int s = size();
    for (int i = 0; i < s; i++)
        deleteGui(i);
}

//---------------------------------------------------------
//   guiVisible
//---------------------------------------------------------

bool Pipeline::guiVisible(int idx)
{
    return false;
}

//---------------------------------------------------------
//   hasNativeGui
//---------------------------------------------------------

bool Pipeline::hasNativeGui(int idx)
{
    return false;
}

//---------------------------------------------------------
//   showNativeGui
//---------------------------------------------------------

void Pipeline::showNativeGui(int idx, bool flag)
{
}

//---------------------------------------------------------
//   nativeGuiVisible
//---------------------------------------------------------

bool Pipeline::nativeGuiVisible(int idx)
{
    return false;
}

void Pipeline::updateGuis()
{
}
