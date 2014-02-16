#=============================================================================
#  LOS
#  Libre Octave Studio
#  $Id:$
#
#  Copyright (C) 2012-2014 by Filipe Coelho
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

QT4_WRAP_CPP(theme_mocs
    CarlaStyle.hpp
    CarlaStyleAnimations.hpp
    CarlaStylePrivate.hpp
)

##
## List of source files to compile
##

file(GLOB theme_source_files
    CarlaStyle.cpp
)

##
## Define target
##

add_library(theme STATIC
    ${theme_source_files}
    ${theme_mocs}
)

##
## Compilation flags and target name
##

set_target_properties(theme
    PROPERTIES
    OUTPUT_NAME los_theme
)
