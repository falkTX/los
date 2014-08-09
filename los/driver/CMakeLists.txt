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
## List of source files to compile
##

file (GLOB driver_source_files
    alsamidi.cpp
    alsatimer.cpp
    dummyaudio.cpp
    jack.cpp
    jackmidi.cpp
)

##
## Define target
##

add_library(driver STATIC
    ${driver_source_files}
)

##
## Append to the list of translations
##

set(FILES_TO_TRANSLATE
    ${FILES_TO_TRANSLATE}
    ${driver_source_files}
    CACHE INTERNAL ""
)

##
## Compilation flags and target name
##

set_target_properties(driver
    PROPERTIES OUTPUT_NAME los_driver
)
