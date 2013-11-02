#  LOS
#  Libre Octave Studio
#  $Id:$
#
#  Copyright (C) 2002-2008 by Werner Schweer and others
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

include(CheckIncludeFiles)
include(FindPkgConfig)
include(cmake/Summary.cmake)
include(cmake/TargetDoc.cmake)

project(oom)

CMAKE_MINIMUM_REQUIRED(VERSION 2.4.1)
if (COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

if (NOT CMAKE_INSTALL_LIBDIR)
    SET(CMAKE_INSTALL_LIBDIR "${CMAKE_INSTALL_PREFIX}/lib")
endif (NOT CMAKE_INSTALL_LIBDIR)

IF(NOT DEFINED SHARE_INSTALL_PREFIX)
    SET(SHARE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}/share")
ENDIF(NOT DEFINED SHARE_INSTALL_PREFIX)

#set(CMAKE_BUILD_TYPE debug)
#set(CMAKE_BUILD_TYPE release)

# If no CMAKE_BUILD_TYPE is given on the command line,
#  cmake either uses the cached value, or 'empty' (plain un-opt build). 
# And yet the only way a user can reset a cached CMAKE_BUILD_TYPE
#  is with "-DCMAKE_BUILD_TYPE=". So we cannot interfere with this.
# We should probably not attempt to do this at all.
# Installation instructions now updated to be more specific, give options.
#endif (NOT DEFINED CMAKE_BUILD_TYPE)
#if (NOT CMAKE_BUILD_TYPE)
#    message("No CMAKE_BUILD_TYPE specified. Setting to Release.")
#     set(CMAKE_BUILD_TYPE release)
#endif (NOT CMAKE_BUILD_TYPE)

# for debugging the make system uncomment next line:
# set(CMAKE_VERBOSE_MAKEFILE ON)

##
## set internal midi buffer sizes
##
if(NOT DEFINED OOM_MIDI_FIFO_SIZE)
    set(OOM_MIDI_FIFO_SIZE 2048)
endif(NOT DEFINED OOM_MIDI_FIFO_SIZE)

if(NOT DEFINED OOM_MIDI_REC_FIFO_SIZE)
    set(OOM_MIDI_REC_FIFO_SIZE 256)
endif(NOT DEFINED OOM_MIDI_REC_FIFO_SIZE)

## TODO - description
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)
set(CMAKE_SKIP_RULE_DEPENDENCY TRUE)

# The OOMidi version number.
SET(OOMidi_VERSION_MAJOR 2014)
SET(OOMidi_VERSION_MINOR 1)
SET(OOMidi_VERSION_PATCH alpha1)
SET(OOMidi_VERSION "${OOMidi_VERSION_MAJOR}.${OOMidi_VERSION_MINOR}.${OOMidi_VERSION_PATCH}")
SET(OOMidi_VERSION_FULL  "${OOMidi_VERSION}")
SET(OOMidi_INSTALL_NAME  "oom-${OOMidi_VERSION}")
SET(OOMidi_EXEC_NAME     "oomidi")
SET(OOMidi_INSTALL_PATH "${CMAKE_INSTALL_PREFIX}/bin/${OOMidi_EXEC_NAME}")
SET(OOMIDI_DEPS_DIR "${PROJECT_SOURCE_DIR}/dependencies")

## The library where we will install the shared components:
SET(OOMidi_MODULES_DIR ${CMAKE_INSTALL_LIBDIR}/${OOMidi_INSTALL_NAME}/modules)

## Top documentation dir
IF(NOT DEFINED OOMidi_DOC_DIR)
    SET(OOMidi_DOC_DIR ${SHARE_INSTALL_PREFIX}/doc/${OOMidi_INSTALL_NAME}/)
ENDIF(NOT DEFINED OOMidi_DOC_DIR)

## Data directory
SET(OOMidi_SHARE_DIR ${SHARE_INSTALL_PREFIX}/${OOMidi_INSTALL_NAME})

## Lib directory
SET(OOMidi_LIB_DIR ${CMAKE_INSTALL_LIBDIR}/${OOMidi_INSTALL_NAME})

option ( UPDATE_TRANSLATIONS "Update source translation share/locale/*.ts files
         (WARNING: This will modify the .ts files in the source tree!!)"          OFF)

##
##  look for Qt4
##
set(QT_MIN_VERSION "4.3.0")
set(QT_USE_QTSVG TRUE)
set(QT_USE_QTXML TRUE)

find_package(Qt4)

if (NOT QT4_FOUND)
      message(FATAL_ERROR "Fatal error: QT (version >= 4.3.0) required.\n"
      "Cmake tries to detect QT4 by searching for 'qmake' in your PATH\n"
      "If you have QT4 installed, make sure qmake is found in your PATH."
      )
endif (NOT QT4_FOUND)

include(${QT_USE_FILE})


##
## find doxygen
##    TODO

##
## alsa >= 0.9.0
##
if (APPLE)
    message("Disabling ALSA support due to OS X build.")
else (APPLE)
    PKG_CHECK_MODULES(ALSA REQUIRED alsa>=0.9.0)
    include_directories(${ALSA_INCLUDE_DIRS})
endif (APPLE)

##
## find sndfile >= 1.0.0
##
PKG_CHECK_MODULES(SNDFILE REQUIRED sndfile>=1.0.0)
include_directories(${SNDFILE_INCLUDE_DIRS})

##
## find libsamplerate >= 0.1.0
##
PKG_CHECK_MODULES(SAMPLERATE REQUIRED samplerate>=0.1.0)
include_directories(${SAMPLERATE_INCLUDE_DIRS})

##
## find libuuid
##
PKG_CHECK_MODULES(UUID REQUIRED uuid>=0.0.1)
include_directories(${UUID_INCLUDE_DIRS})

##
## find jack >= 0.103.0
##
PKG_CHECK_MODULES(JACK REQUIRED jack>=0.103)
include_directories(${JACK_INCLUDE_DIRS})

##
## find if jack is jack2
##
PKG_CHECK_MODULES(JACK2 jack>=1.9.5)
if (JACK2_FOUND)
    set(JACK2_SUPPORT ON)
else (JACK2_FOUND)
    set(JACK2_SUPPORT OFF)
endif (JACK2_FOUND)

##
## Set the plugin headers path
##
include_directories(${OOMIDI_PLUGIN_INCLUDES})

##
## TODO
##
CHECK_INCLUDE_FILES (wordexp.h HAVE_WORDEXP)

##
## TODO
##
## Optimizations
##
SET(USE_SSE false)

##
## produce config.h file
##
configure_file(
    ${PROJECT_SOURCE_DIR}/config.h.in
    ${PROJECT_BINARY_DIR}/config.h
)

##
## custom build commands
##
add_custom_command(
    OUTPUT ${PROJECT_BINARY_DIR}/all.h
    COMMAND cp ${PROJECT_SOURCE_DIR}/all.h ${PROJECT_BINARY_DIR}/all.h
    DEPENDS ${PROJECT_SOURCE_DIR}/all.h
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
)

add_custom_command(
    OUTPUT ${PROJECT_BINARY_DIR}/all-pic.h
    COMMAND cp ${PROJECT_SOURCE_DIR}/all.h ${PROJECT_BINARY_DIR}/all-pic.h
    DEPENDS ${PROJECT_SOURCE_DIR}/all.h
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
)

add_custom_command(
    OUTPUT ${PROJECT_BINARY_DIR}/all-pic-debug.h
    COMMAND cp ${PROJECT_SOURCE_DIR}/all.h ${PROJECT_BINARY_DIR}/all-pic-debug.h
    DEPENDS ${PROJECT_SOURCE_DIR}/all.h
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
)

##
## include dirs
##
include_directories(
    .
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/oom
    ${PROJECT_SOURCE_DIR}/oom/widgets
    ${PROJECT_BINARY_DIR}
    ${PROJECT_BINARY_DIR}/oom
    ${PROJECT_BINARY_DIR}/oom/ctrl
    ${PROJECT_BINARY_DIR}/oom/instruments
    ${PROJECT_BINARY_DIR}/oom/widgets
)

##
## If the cmake version includes cpack, use it
##
IF(EXISTS "${CMAKE_ROOT}/Modules/CPack.cmake")
  IF(EXISTS "${CMAKE_ROOT}/Modules/InstallRequiredSystemLibraries.cmake")
      SET(CMAKE_INSTALL_MFC_LIBRARIES 1)
      INCLUDE(InstallRequiredSystemLibraries)
  ENDIF(EXISTS "${CMAKE_ROOT}/Modules/InstallRequiredSystemLibraries.cmake")

  SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "OOMidi is a virtual audio studio environment")
  SET(CPACK_PACKAGE_VENDOR "The Open Octave Project and others")
  SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
  SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
  SET(CPACK_PACKAGE_VERSION_MAJOR "${OOMidi_VERSION_MAJOR}")
  SET(CPACK_PACKAGE_VERSION_MINOR "${OOMidi_VERSION_MINOR}")
  SET(CPACK_PACKAGE_VERSION_PATCH "${OOMidi_VERSION_PATCH}")
  SET(CPACK_PACKAGE_INSTALL_DIRECTORY "CMake ${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}")
  SET(CPACK_SOURCE_PACKAGE_FILE_NAME "${OOMidi_INSTALL_NAME}")

  # Don't pack the binary tree and the subversion directories
  SET(CPACK_SOURCE_IGNORE_FILES "/\\\\.git/;/\\\\.gitignore;${CMAKE_BINARY_DIR}/*")

  SET(CPACK_SYSTEM_NAME ${CMAKE_SYSTEM_NAME})

  SET(CPACK_PACKAGE_FILE_NAME "${CPACK_SOURCE_PACKAGE_FILE_NAME}-${CPACK_SYSTEM_NAME}")
  SET(CPACK_STRIP_FILES "bin/oomidi")
  SET(CPACK_PACKAGE_EXECUTABLES "oomidi" "OOMidi")
  INCLUDE(CPack)
ENDIF(EXISTS "${CMAKE_ROOT}/Modules/CPack.cmake")

set(CMAKE_C_FLAGS           "-std=c99 ${CMAKE_C_FLAGS}") # -fvisibility=hidden
set(CMAKE_CXX_FLAGS         "-Wall -Wextra -Winvalid-pch -fexceptions ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -fomit-frame-pointer -ffast-math -fstrength-reduce ${CMAKE_CXX_FLAGS_RELEASE}")
set(CMAKE_CXX_FLAGS_DEBUG   "-g -DQT_DEBUG ${CMAKE_CXX_FLAGS_DEBUG}")

##
## subdirs
## NOTE: share/ directory needs to be at the end so that the translations
##       are scanned before coming to share/locale
##
subdirs(al awl man oom packaging patchcanvas utils share)

##
## Install doc files
##
file(GLOB doc_files
    AUTHORS
    COPYING
    README
)
INSTALL(FILES ${doc_files} DESTINATION ${OOMidi_DOC_DIR})

##
## Uninstall support
##
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake"
  IMMEDIATE @ONLY
)

add_custom_target(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake")

##
## Show a summary of what we got
##
summary_show()

##
## Make the user aware of what type they're building.
##
if (NOT CMAKE_BUILD_TYPE)
    message(" Build type: CMAKE_BUILD_TYPE is empty. Plain un-optimized build.")
else (NOT CMAKE_BUILD_TYPE)
    message(" Build type: " ${CMAKE_BUILD_TYPE} )
endif (NOT CMAKE_BUILD_TYPE)
message("")
