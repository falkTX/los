/*
 * LOS: Libre Octave Studio
 *
 * Copyright (C) 1999-2004 Werner Schweer <ws@seh.de>
 * Copyright (C) 2006-2011 Remon Sijrier
 * Copyright (C) 2011-2012 Andrew Williams
 * Copyright (C) 2011-2012 Christopher Cherrett
 * Copyright (C) 2012-2014 Filipe Coelho <falktx@falktx.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * For a full copy of the GNU General Public License see the COPYING file.
 */

#ifndef LOS_GLOBAL_DEFS_HPP_INCLUDED
#define LOS_GLOBAL_DEFS_HPP_INCLUDED

#include "defines.h"

static const int kMaxAudioChannels = 2;    // max audio channels
static const int kMaxMidiPorts     = 1024; // max number of midi ports
static const int kMaxMidiChannels  = 16;   // channels per port

enum MidiType {
    MIDI_TYPE_NULL = 0,
    MIDI_TYPE_GM   = 1, // General Midi
    MIDI_TYPE_GS   = 2, // Roland GS
    MIDI_TYPE_XG   = 3  // Yamaha XG
};

struct SamplerData {
    int samplerChannel;
    int audioDevice;
    int audioChannel;
    int midiDevice;
    int midiPort;

    SamplerData() noexcept
        : samplerChannel(-1),
          audioDevice(-1),
          audioChannel(-1),
          midiDevice(-1),
          midiPort(-1) {}
};

#endif // LOS_GLOBAL_DEFS_HPP_INCLUDED
