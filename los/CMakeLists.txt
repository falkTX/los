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

include(${PROJECT_SOURCE_DIR}/pch.txt)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SubDirs
    Composer
    ctrl
    driver
    instruments
    liste
    marker
    master
    midiedit
    midiplugins
    theme
    widgets
)

subdirs(${SubDirs})

##
## Expand Qt macros in source files
##

QT4_WRAP_CPP(los_moc_headers
    app.h
    cobject.h
    conf.h
    AbstractMidiEditor.h
    miditransform.h
    song.h
    thread.h
    transport.h
    transpose.h
    traverso_shared/TConfig.h
    traverso_shared/AddRemoveCtrlValues.h
    traverso_shared/CommandGroup.h
    traverso_shared/OOMCommand.h
    midimonitor.h
    TrackManager.h
    NameValidator.h
)

##
## Resource files
##

QT4_ADD_RESOURCES(los_qrc_files
    los.qrc
)

file(GLOB core_source_files
    app.cpp
    audio.cpp
    audioprefetch.cpp
    cobject.cpp
    conf.cpp
    ctrl.cpp
    event.cpp
    eventlist.cpp
    exportmidi.cpp
    gconfig.cpp
    globals.cpp
    help.cpp
    helper.cpp
    importmidi.cpp
    memory.cpp
    midi.cpp
    midictrl.cpp
    mididev.cpp
    AbstractMidiEditor.cpp
    midievent.cpp
    midifile.cpp
    midiport.cpp
    midiseq.cpp
    miditransform.cpp
    mpevent.cpp
    node.cpp
    part.cpp
    pos.cpp
    route.cpp
    seqmsg.cpp
    shortcuts.cpp
    sig.cpp
    song.cpp
    songfile.cpp
    tempo.cpp
    thread.cpp
    track.cpp
    trackview.cpp
    transport.cpp
    transpose.cpp
    utils.cpp
    undo.cpp
    xml.cpp
    traverso_shared/TConfig.cpp
    traverso_shared/AddRemoveCtrlValues.cpp
    traverso_shared/CommandGroup.cpp
    traverso_shared/OOMCommand.cpp
    midimonitor.cpp
    ccinfo.cpp
    TrackManager.cpp
    NameValidator.cpp
)

file(GLOB main_source_files
    main.cpp
)

file(GLOB icons_source_files
    icons.cpp
)

##
## Define target
##

add_library(core STATIC
    ${los_qrc_files}
    ${los_moc_headers}
    ${core_source_files}
)

add_executable(los
    ${main_source_files}
)

add_library(icons STATIC
    ${icons_source_files}
)

set(FILES_TO_TRANSLATE
    ${FILES_TO_TRANSLATE}
    ${main_source_files}
    ${core_source_files}
    CACHE INTERNAL ""
)

##
## Directories that will be passed to the compiler with -I flag
##

include_directories(
    Composer
    driver
    instruments
    liste
    midiedit
    midiplugins
    widgets
)

# Make sure to include the uic generated headers paths.
include_directories(
#     ${CMAKE_CURRENT_BINARY_DIR}/Composer
#     ${CMAKE_CURRENT_BINARY_DIR}/driver
#     ${CMAKE_CURRENT_BINARY_DIR}/instruments
#     ${CMAKE_CURRENT_BINARY_DIR}/liste
#     ${CMAKE_CURRENT_BINARY_DIR}/midiedit
#     ${CMAKE_CURRENT_BINARY_DIR}/midiplugins
    ${CMAKE_CURRENT_BINARY_DIR}/widgets
)

##
## Compilation flags and target name
##

set_target_properties(core
    PROPERTIES COMPILE_FLAGS "${PYREM_CPP_FLAGS} -include ${PROJECT_BINARY_DIR}/all.h -fPIC -fopenmp"
    LINK_FLAGS "-fopenmp"
    OUTPUT_NAME los_core
)

set_target_properties(los
    PROPERTIES OUTPUT_NAME ${LOS_EXEC_NAME}
)

set_target_properties(icons
    PROPERTIES OUTPUT_NAME los_icons
)

##
## Linkage
##

target_link_libraries(core
    Composer
    ctrl
    driver
    icons
    instruments
    liste
    marker
    master
    midiedit
    midiplugins
    widgets
)

target_link_libraries(los
# core
    core
# all libs
    al
    Composer
    awl
    ctrl
    driver
    icons
    instruments
    liste
    marker
    master
    midiedit
    midiplugins
    theme
    widgets
# needed for linking
    core
    ctrl
    widgets
# system libs
    ${QT_LIBRARIES}
    ${ALSA_LIBRARIES}
    ${JACK_LIBRARIES}
    ${UUID_LIBRARIES}
    pthread
)

##
## Install location
##

install(TARGETS los
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/
)
