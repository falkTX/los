//=========================================================
//  OOMidi
//  OpenOctave Midi and Audio Editor
//  $Id: plugin.h,v 1.9.2.13 2009/12/06 01:25:21 terminator356 Exp $
//
//  (C) Copyright 2000 Werner Schweer (ws@seh.de)
//  (C) Copyright 2012 Filipe Coelho (falktx@openoctave.org)
//=========================================================

#ifndef __PLUGIN_H__
#define __PLUGIN_H__

#include "audio.h"
#include "ctrl.h"
#include "globals.h"

#include "mididev.h"
#include "instruments/minstrument.h"

#include <list>
#include <vector>
#include <math.h>
#include <stdint.h>
#include <QFileInfo>
#include <QMutex>

class AudioTrack;
class BasePlugin;
class PluginI;
class PluginGui;
class Xml;

//---------------------------------------------------------
//   Pipeline
//    chain of connected efx inserts
//---------------------------------------------------------

const int PipelineDepth = 100;

class Pipeline : public std::vector<BasePlugin*>
{
public:
    Pipeline();
    ~Pipeline();

    int addPlugin(BasePlugin* plugin, int index);
    void remove(int index);
    void removeAll();

    bool isActive(int idx) const;
    void setActive(int, bool);

    void setChannels(int);

    QString label(int idx) const;
    QString name(int idx) const;

    bool empty(int idx) const;
    void move(int idx, bool up);
    void apply(int ports, uint32_t nframes, float** buffer);

    void showGui(int, bool);
    void deleteGui(int idx);
    void deleteAllGuis();
    bool guiVisible(int);

    bool hasNativeGui(int);
    void showNativeGui(int, bool);
    bool nativeGuiVisible(int);

    void updateGuis();

private:
    float* buffer[MAX_CHANNELS];
};

typedef Pipeline::iterator iPluginI;
typedef Pipeline::const_iterator ciPluginI;

//---------------------------------------------------------

#endif

