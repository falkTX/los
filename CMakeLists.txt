#=============================================================================
#  LOS
#  Libre Octave Studio
#  $Id:$
#
#  Copyright (C) 2002-2008 by Werner Schweer and others
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
## include cmake stuff
##

include(CheckIncludeFiles)
include(FindPkgConfig)
include(cmake/Summary.cmake)
include(cmake/TargetDoc.cmake)

##
## set cmake stuff
##

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

if (COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

if (NOT DEFINED CMAKE_INSTALL_PREFIX)
    SET(CMAKE_INSTALL_PREFIX "/usr/local")
endif (NOT DEFINED CMAKE_INSTALL_PREFIX)

if (NOT DEFINED CMAKE_INSTALL_LIBDIR)
    SET(CMAKE_INSTALL_LIBDIR "${CMAKE_INSTALL_PREFIX}/lib")
endif (NOT DEFINED CMAKE_INSTALL_LIBDIR)

IF(NOT DEFINED SHARE_INSTALL_PREFIX)
    SET(SHARE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}/share")
ENDIF(NOT DEFINED SHARE_INSTALL_PREFIX)

set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -std=c99   -Wall -Wextra")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall -Wextra")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2 -DNDEBUG -fomit-frame-pointer -ffast-math -fstrength-reduce")
set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG}   -O0 -DDEBUG -DQT_DEBUG -g")

# set(CMAKE_CXX_FLAGS         " -fexceptions ${CMAKE_CXX_FLAGS}")

set(CMAKE_INCLUDE_CURRENT_DIR  TRUE)
set(CMAKE_SKIP_RULE_DEPENDENCY TRUE)
set(CMAKE_VERBOSE_MAKEFILE     ON)

##
## set debug or release
##

if (NOT DEFINED CMAKE_BUILD_TYPE)
    message("No CMAKE_BUILD_TYPE specified. Setting to Debug.")
    set(CMAKE_BUILD_TYPE debug)
endif (NOT DEFINED CMAKE_BUILD_TYPE)

##
## define project
##

project(los)

SET(LOS_VERSION_MAJOR 2014)
SET(LOS_VERSION_MINOR 0)
SET(LOS_VERSION_PATCH 0)
SET(LOS_VERSION       "${LOS_VERSION_MAJOR}.${LOS_VERSION_MINOR}.${LOS_VERSION_PATCH}")

SET(LOS_EXEC_NAME     "los-${LOS_VERSION_MAJOR}")
SET(LOS_INSTALL_NAME  "los-${LOS_VERSION}")
SET(LOS_INSTALL_PATH  "${CMAKE_INSTALL_PREFIX}/bin/${LOS_INSTALL_NAME}")

## Doc directory
SET(LOS_DOC_DIR   ${SHARE_INSTALL_PREFIX}/doc/${LOS_INSTALL_NAME})

## Lib directory
SET(LOS_LIB_DIR   ${CMAKE_INSTALL_LIBDIR}/${LOS_INSTALL_NAME})

## Share directory
SET(LOS_SHARE_DIR ${SHARE_INSTALL_PREFIX}/${LOS_INSTALL_NAME})

##
## set internal midi buffer sizes
##

if(NOT DEFINED LOS_MIDI_FIFO_SIZE)
    set(LOS_MIDI_FIFO_SIZE 2048)
endif(NOT DEFINED LOS_MIDI_FIFO_SIZE)

if(NOT DEFINED LOS_MIDI_REC_FIFO_SIZE)
    set(LOS_MIDI_REC_FIFO_SIZE 256)
endif(NOT DEFINED LOS_MIDI_REC_FIFO_SIZE)

##
## check build dependencies
##

## TODO
CHECK_INCLUDE_FILES(wordexp.h HAVE_WORDEXP)

## alsa >= 0.9.0
PKG_CHECK_MODULES(ALSA REQUIRED alsa>=0.9.0)
include_directories(${ALSA_INCLUDE_DIRS})

## jack >= 0.103.0
PKG_CHECK_MODULES(JACK REQUIRED jack>=0.103)
include_directories(${JACK_INCLUDE_DIRS})

## uuid
PKG_CHECK_MODULES(UUID REQUIRED uuid>=0.0.1)
include_directories(${UUID_INCLUDE_DIRS})

#################################################
## Qt4

set(QT_MIN_VERSION "4.3.0")
set(QT_USE_QTXML TRUE)

find_package(Qt4)

if (NOT QT4_FOUND)
    message(FATAL_ERROR "Fatal error: QT (version >= 4.3.0) required.\n"
                        "cmake tries to detect QT4 by searching for 'qmake' in your PATH.\n"
                        "If you have Qt4 installed, make sure qmake is found in your PATH.")
endif (NOT QT4_FOUND)

include(${QT_USE_FILE})

#################################################
##  Qt5

# find_package(Qt5Core REQUIRED)
# find_package(Qt5Gui REQUIRED)
# find_package(Qt5LinguistTools REQUIRED)
# find_package(Qt5Xml REQUIRED)
# find_package(Qt5Widgets REQUIRED)

# add_definitions(${Qt5Widgets_DEFINITIONS})
# include_directories(${Qt5Xml_INCLUDE_DIRS})
# include_directories(${Qt5Widgets_INCLUDE_DIRS})

# set(CMAKE_AUTOMOC ON)

# if (Qt5_POSITION_INDEPENDENT_CODE)
#     set(CMAKE_POSITION_INDEPENDENT_CODE ON)
# endif()

##
## produce config.h file
##

configure_file(
    ${PROJECT_SOURCE_DIR}/config.h.in
    ${PROJECT_BINARY_DIR}/config.h
)

##
## install doc files
##

file(GLOB doc_files
    AUTHORS
    COPYING
    README
)
INSTALL(FILES ${doc_files} DESTINATION ${LOS_DOC_DIR})

##
## set include dirs
##

include_directories(
    .
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/los
    ${PROJECT_SOURCE_DIR}/los/widgets
    ${PROJECT_BINARY_DIR}
    ${PROJECT_BINARY_DIR}/los
    ${PROJECT_BINARY_DIR}/los/ctrl
    ${PROJECT_BINARY_DIR}/los/instruments
    ${PROJECT_BINARY_DIR}/los/widgets
)

##
## set subdirs
## NOTE: share/ directory needs to be at the end so that the translations
##       are scanned before coming to share/locale
##

subdirs(al awl man los packaging utils share)
# subdirs(al awl man los packaging patchcanvas utils share)

##
## set uninstall
##

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake"
  IMMEDIATE @ONLY
)

add_custom_target(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake")

##
## set options
##

# option(UPDATE_TRANSLATIONS "Update source translation share/locale/*.ts files
#                             (WARNING: This will modify the .ts files in the source tree!!)" OFF)
