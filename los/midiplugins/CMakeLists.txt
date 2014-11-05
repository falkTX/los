#=============================================================================
#  LOS
#  Libre Octave Studio
#  $Id:$
#
#  Copyright (C) 2002-2006 by Werner Schweer and others
#  Copyright (C) 2012-2013 by Filipe Coelho
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License version 2.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#=============================================================================

##
## Expand Qt macros in source files
##

QT4_WRAP_CPP(midiplugins_mocs
    midifilterimpl.h
    midiitransform.h
    mittranspose.h
    mrconfig.h
)

##
## UI files
##

file(GLOB midiplugins_ui_files
    midifilter.ui
    mrconfigbase.ui
)

QT4_WRAP_UI(midiplugins_ui_headers ${midiplugins_ui_files})

##
## List of source files to compile
##

file(GLOB midiplugins_source_files
    midifilterimpl.cpp
    midiitransform.cpp
    mitplugin.cpp
    mittranspose.cpp
    mrconfig.cpp
)

##
## Define target
##

add_library(midiplugins STATIC
    ${midiplugins_mocs}
    ${midiplugins_ui_headers}
    ${midiplugins_source_files}
)

##
## Append to the list of translations
##

set(FILES_TO_TRANSLATE
    ${FILES_TO_TRANSLATE}
    ${midiplugins_source_files}
    ${midiplugins_ui_files}
    CACHE INTERNAL ""
)

##
## Compilation flags and target name
##

set_target_properties(midiplugins
    PROPERTIES OUTPUT_NAME los_midiplugins
)
