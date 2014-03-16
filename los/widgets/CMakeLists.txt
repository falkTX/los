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

QT4_WRAP_CPP(widget_mocs
    toolbars/transporttools.h
    toolbars/edittools.h
    toolbars/epictools.h
    toolbars/looptools.h
    toolbars/misctools.h
    toolbars/feedbacktools.h
    toolbars/tools.h
    aboutbox_impl.h
    action.h
    bigtime.h
    canvas.h
    comboQuant.h
    confmport.h
    comment.h
    commentdock.h
    ctrlcombo.h
    dentry.h
    doublelabel.h
    filedialog.h
    gatetime.h
    genset.h
    intlabel.h
    knob.h
    lcombo.h
    gcombo.h
    mtscale.h
    pcscale.h
    tvieweditor.h
    tviewdock.h
    tracklistview.h
    trackheader.h
    conductor/pctablemodel.h
    conductor/pctable.h
    conductor/Conductor.h
    nentry.h
    noteinfo.h
    pitchedit.h
    pitchlabel.h
    popupmenu.h
    poslabel.h
    projectcreateimpl.h
    scrollscale.h
    shortcutcapturedialog.h
    shortcutconfig.h
    siglabel.h
    sigscale.h
    slider.h
    sliderbase.h
    songinfo.h
    spinbox.h
    spinboxFP.h
    tempolabel.h
    velocity.h
    view.h
    vscale.h
    tviewmenu.h
    tablespinner.h
    instrumenttree.h
    instrumentdelegate.h
    instrumentcombo.h
    instrumentmenu.h
    keymapmenu.h
    midiassign.h
    midiportdelegate.h
    midipresetdelegate.h
    ccdelegate.h
    ccedit.h
    CreateTrackDialog.h
    StretchDialog.h
    TimeHeader.h
    TrackInstrumentMenu.h
    TempoCanvas.h
    TempoHeader.h
    DoubleSlider.h
    meter.h
)

##
## UI files
##

file(GLOB widgets_ui_files
    aboutbox.ui
    commentbase.ui
    commentdockbase.ui
    configmidifilebase.ui
    editnotedialogbase.ui
    editsysexdialogbase.ui
    fdialogbuttons.ui
    gatetimebase.ui
    gensetbase.ui
    itransformbase.ui
    mittransposebase.ui
    conductor/ConductorBase.ui
    projectcreate.ui
    shortcutcapturedialogbase.ui
    shortcutconfigbase.ui
    songinfo.ui
    synthconfigbase.ui
    transformbase.ui
    transposebase.ui
    velocitybase.ui
    trackvieweditorbase.ui
    tviewdockbase.ui
    trackheaderbase.ui
    routemapbase.ui
    midiassignbase.ui
    cceditbase.ui
    createtrackbase.ui
    stretchdialog.ui
)

QT4_WRAP_UI(widget_ui_headers ${widgets_ui_files})

##
## List of source files to compile
##

file(GLOB widgets_source_files
    toolbars/tools.cpp
    toolbars/transporttools.cpp
    toolbars/edittools.cpp
    toolbars/epictools.cpp
    toolbars/looptools.cpp
    toolbars/misctools.cpp
    toolbars/feedbacktools.cpp
    aboutbox_impl.cpp
    bigtime.cpp
    canvas.cpp
    citem.cpp
    comboQuant.cpp
    confmport.cpp
    comment.cpp
    commentdock.cpp
    ctrlcombo.cpp
    dentry.cpp
    dimap.cpp
    doublelabel.cpp
    drange.cpp
    filedialog.cpp
    gatetime.cpp
    genset.cpp
    intlabel.cpp
    knob.cpp
    lcombo.cpp
    gcombo.cpp
    mmath.cpp
    conductor/Conductor.cpp
    tvieweditor.cpp
    tviewdock.cpp
    tracklistview.cpp
    trackheader.cpp
    mtscale.cpp
    pcscale.cpp
    conductor/pctablemodel.cpp
    conductor/pctable.cpp
    nentry.cpp
    noteinfo.cpp
    pitchedit.cpp
    pitchlabel.cpp
    popupmenu.cpp
    poslabel.cpp
    projectcreateimpl.cpp
    scldiv.cpp
    scldraw.cpp
    sclif.cpp
    scrollscale.cpp
    shortcutcapturedialog.cpp
    shortcutconfig.cpp
    siglabel.cpp
    sigscale.cpp
    slider.cpp
    sliderbase.cpp
    spinbox.cpp
    spinboxFP.cpp
    tempolabel.cpp
    velocity.cpp
    view.cpp
    vscale.cpp
    tviewmenu.cpp
    tablespinner.cpp
    instrumenttree.cpp
    instrumentdelegate.cpp
    instrumentcombo.cpp
    instrumentmenu.cpp
    keymapmenu.cpp
    midiassign.cpp
    midiportdelegate.cpp
    midipresetdelegate.cpp
    ccdelegate.cpp
    ccedit.cpp
    CreateTrackDialog.cpp
    StretchDialog.cpp
    TimeHeader.cpp
    TrackInstrumentMenu.cpp
    TempoCanvas.cpp
    TempoHeader.cpp
    DoubleSlider.cpp
    meter.cpp
    menutitleitem.cpp
)

##
## Define target
##

add_library(widgets STATIC
    ${widget_ui_headers}
    ${widget_mocs}
    ${widgets_source_files}
)

##
## Append to the list of translations
##

set(FILES_TO_TRANSLATE
    ${FILES_TO_TRANSLATE}
    ${widgets_source_files}
    ${widgets_ui_files}
    CACHE INTERNAL ""
)

##
## Compilation flags and target name
##

set_target_properties(widgets
    PROPERTIES COMPILE_FLAGS "-include ${PROJECT_BINARY_DIR}/all.h ${MUSECXXFLAGS} -fPIC"
    OUTPUT_NAME los_widgets
)
