//=========================================================
//  LOS
//  Libre Octave Studio
//  $Id: icons.cpp,v 1.13.2.8 2009/11/14 03:37:48 terminator356 Exp $
//
//  (C) Copyright 1999/2000 Werner Schweer (ws@seh.de)
//=========================================================

#include "globals.h"

#include <QIcon>

#include "xpm/track_comment.xpm"
#include "xpm/audio_bounce_to_file.xpm"
#include "xpm/audio_bounce_to_track.xpm"
#include "xpm/audio_restartaudio.xpm"
#include "xpm/automation_clear_data.xpm"
#include "xpm/automation_take_snapshot.xpm"
#include "xpm/edit_midi.xpm"
#include "xpm/midi_edit_instrument.xpm"
#include "xpm/midi_init_instr.xpm"
#include "xpm/midi_inputplugins.xpm"
#include "xpm/midi_inputplugins_midi_input_filter.xpm"
#include "xpm/midi_inputplugins_midi_input_transform.xpm"
#include "xpm/midi_inputplugins_remote_control.xpm"
#include "xpm/midi_inputplugins_transpose.xpm"
#include "xpm/midi_inputplugins_random_rhythm_generator.xpm"
#include "xpm/midi_local_off.xpm"
#include "xpm/midi_reset_instr.xpm"
#include "xpm/midi_thru_off3.xpm"
#include "xpm/midi_thru_on3.xpm"
#include "xpm/settings_appearance_settings.xpm"
#include "xpm/settings_configureshortcuts.xpm"
#include "xpm/settings_follow_song.xpm"
#include "xpm/settings_globalsettings.xpm"
#include "xpm/settings_midifileexport.xpm"
#include "xpm/settings_midiport_softsynths.xpm"
#include "xpm/settings_midisync.xpm"
#include "xpm/view_bigtime_window.xpm"
#include "xpm/view_cliplist.xpm"
#include "xpm/view_marker.xpm"
#include "xpm/view_transport_window.xpm"
#include "xpm/note.xpm"
#include "xpm/note1.xpm"
#include "xpm/stick.xpm"
#include "xpm/wave.xpm"
#include "xpm/synth.xpm"
#include "xpm/quant.xpm"
#include "xpm/fileprint.xpm"
#include "xpm/filesave.xpm"
#include "xpm/filesaveas.xpm"
#include "xpm/fileopen.xpm"
#include "xpm/fileprintS.xpm"
#include "xpm/filesaveS.xpm"
#include "xpm/filesaveasS.xpm"
#include "xpm/fileopenS.xpm"
#include "xpm/master.xpm"
#include "xpm/filenew.xpm"
#include "xpm/filenewS.xpm"
#include "xpm/home.xpm"
#include "xpm/back.xpm"
#include "xpm/forward.xpm"

#include "xpm/up.xpm"
#include "xpm/down.xpm"
#include "xpm/bold.xpm"
#include "xpm/italic.xpm"
#include "xpm/underlined.xpm"
#include "xpm/gv.xpm"
#include "xpm/midiin.xpm"
#include "xpm/sysex.xpm"
#include "xpm/ctrl.xpm"
#include "xpm/meta.xpm"
#include "xpm/pitch.xpm"
#include "xpm/cafter.xpm"
#include "xpm/pafter.xpm"
#include "xpm/flag.xpm"
#include "xpm/flagS.xpm"
#include "xpm/lock.xpm"
#include "xpm/toc.xpm"
#include "xpm/exitS.xpm"

#include "xpm/undo.xpm"
#include "xpm/redo.xpm"
#include "xpm/undoS.xpm"
#include "xpm/redoS.xpm"
#include "xpm/editcutS.xpm"
#include "xpm/editcopyS.xpm"
#include "xpm/editpasteS.xpm"
#include "xpm/editmuteS.xpm"
#include "xpm/editpastecloneS.xpm"
#include "xpm/editpaste2trackS.xpm"
#include "xpm/editpasteclone2trackS.xpm"

#include "xpm/buttondown.xpm"
#include "xpm/configure.xpm"


// next two lines will vanish soon
#include "xpm/solobutton.xpm"
#include "xpm/newmutebutton.xpm"

#include "xpm/redled.xpm"
#include "xpm/darkredled.xpm"
#include "xpm/greendot.xpm"
#include "xpm/bluedot.xpm"
#include "xpm/graydot.xpm"
#include "xpm/off.xpm"
#include "xpm/blacksquare.xpm"
#include "xpm/blacksqcheck.xpm"

#include "xpm/mastertrackS.xpm"
#include "xpm/localoffS.xpm"
#include "xpm/miditransformS.xpm"
#include "xpm/midi_plugS.xpm"
#include "xpm/mustangS.xpm"
#include "xpm/resetS.xpm"
#include "xpm/track_add.xpm"
#include "xpm/track_delete.xpm"
#include "xpm/listS.xpm"
#include "xpm/inputpluginS.xpm"
#include "xpm/cliplistS.xpm"
#include "xpm/initS.xpm"

#include "xpm/addtrack_addmiditrack.xpm"
#include "xpm/addtrack_audiogroup.xpm"
#include "xpm/addtrack_audioinput.xpm"
#include "xpm/addtrack_audiooutput.xpm"
#include "xpm/addtrack_auxsend.xpm"
#include "xpm/addtrack_drumtrack.xpm"
#include "xpm/addtrack_wavetrack.xpm"
#include "xpm/edit_drumms.xpm"
#include "xpm/edit_list.xpm"
#include "xpm/remove_ctrl.xpm"
#include "xpm/edit_wave.xpm"
#include "xpm/edit_mastertrack.xpm"
#include "xpm/edit_pianoroll.xpm"
#include "xpm/edit_score.xpm"
#include "xpm/edit_track_add.xpm"
#include "xpm/edit_track_del.xpm"
#include "xpm/mastertrack_graphic.xpm"
#include "xpm/mastertrack_list.xpm"
#include "xpm/midi_transform.xpm"
#include "xpm/midi_transpose.xpm"
#include "xpm/select.xpm"
#include "xpm/select_all.xpm"
#include "xpm/select_all_parts_on_track.xpm"
#include "xpm/select_inside_loop.xpm"
#include "xpm/select_outside_loop.xpm"

#include "xpm/oom_icon.xpm"

#include "xpm/global.xpm"
#include "xpm/project.xpm"
#include "xpm/user.xpm"

#include "xpm/sine.xpm"
#include "xpm/saw.xpm"

class LOSPixmap : public QPixmap
{
public:
    LOSPixmap(const char* const a, const char* const b)
#if QT_VERSION >= 0x040600
        : QPixmap(QIcon::fromTheme(b, QIcon(QPixmap(a))).pixmap(QPixmap(a).width(),QPixmap(a).height()))
#else
        : QPixmap(a)
#endif
    {
        if (QPixmap::isNull())
            qCritical("Got invalid pixmap %s", a);
    }

    LOSPixmap(const char* const a[], const char* const b)
#if QT_VERSION >= 0x040600
        : QPixmap(QIcon::fromTheme(b, QIcon(QPixmap(a))).pixmap(QPixmap(a).width(),QPixmap(a).height()))
#else
        : QPixmap(a)
#endif
    {
        if (QPixmap::isNull())
            qCritical("Got Invalid XPM");
    }
};

class LOSIcon : public QIcon
{
public:
    LOSIcon(const char* const a, const char* const b)
#if QT_VERSION >= 0x040600
        : QIcon(QIcon::fromTheme(b, QIcon(QPixmap(a))))
#else
        : QIcon(QPixmap(a))
#endif
    {
        if (QIcon::isNull())
            qCritical("Got invalid icon %s", a);
    }

    LOSIcon(const char* const a[], const char* const b)
#if QT_VERSION >= 0x040600
        : QIcon(QIcon::fromTheme(b, QIcon(QPixmap(a))))
#else
        : QIcon(QPixmap(a))
#endif
    {
        if (QIcon::isNull())
            qCritical("Got invalid xpm icon");
    }
};

QPixmap* track_commentIcon;
QPixmap* mastertrackSIcon;
QPixmap* localoffSIcon;
QPixmap* miditransformSIcon;
QPixmap* midi_plugSIcon;
QPixmap* miditransposeSIcon;
QPixmap* commentIcon;
QPixmap* midiThruOnIcon;
QPixmap* midiThruOffIcon;
QPixmap* mixerSIcon;
QPixmap* mustangSIcon;
QPixmap* resetSIcon;
QPixmap* track_addIcon;
QPixmap* track_deleteIcon;
QPixmap* listSIcon;
QPixmap* inputpluginSIcon;
QPixmap* cliplistSIcon;
QPixmap* mixerAudioSIcon;
QPixmap* initSIcon;
QPixmap* songInfoIcon;

QPixmap* addMidiIcon;
QPixmap* addAudioIcon;
QPixmap* addBussIcon;
QPixmap* addOutputIcon;
QPixmap* addInputIcon;
QPixmap* addAuxIcon;
QPixmap* addSynthIcon;

QPixmap* exitIcon;
QPixmap* exit1Icon;
QPixmap* newmuteIcon;
QPixmap* soloIcon;

QPixmap* crosshairIcon;
QPixmap* pointerIcon;
QPixmap* pencilIcon;
QPixmap* pencilCursorIcon;
QPixmap* deleteIcon;
QPixmap* punchinIcon;
QPixmap* punchoutIcon;
QPixmap* punchin1Icon;
QPixmap* punchout1Icon;
QPixmap* loopIcon;
QPixmap* loop1Icon;
QPixmap* playIcon;
QPixmap* auditionIcon;

QPixmap* recordIcon;
QPixmap* stopIcon;
QPixmap* startIcon;
QPixmap* fforwardIcon;
QPixmap* frewindIcon;
QPixmap* dotIcon;
QPixmap* dothIcon;
QPixmap* dot1Icon;
QPixmap* note1Icon;
QPixmap* noteIcon;
QPixmap* stickIcon;
QPixmap* waveIcon;
QPixmap* synthIcon;
QPixmap* markIcon[4];
QPixmap* cutIcon;
QPixmap* steprecIcon;
QPixmap* glueIcon;
QPixmap* drawIcon;
QPixmap* quantIcon;
QPixmap* printIcon;
QPixmap* printIconS;
QPixmap* openIcon;
QPixmap* saveIcon;
QPixmap* saveasIcon;
QPixmap* openIconS;
QPixmap* saveIconS;
QPixmap* saveasIconS;
QPixmap* masterIcon;
QPixmap* filenewIcon;
QPixmap* filenewIconS;
QPixmap* homeIcon;
QPixmap* backIcon;
QPixmap* forwardIcon;
QPixmap* muteIcon;
QPixmap* upIcon;
QPixmap* downIcon;
QPixmap* boldIcon;
QPixmap* italicIcon;
QPixmap* underlinedIcon;
QPixmap* gvIcon;
QPixmap* midiinIcon;
QPixmap* sysexIcon;
QPixmap* ctrlIcon;
QPixmap* metaIcon;
QPixmap* pitchIcon;
QPixmap* cafterIcon;
QPixmap* pafterIcon;
QPixmap* flagIcon;
QPixmap* flagIconS;
QPixmap* flagIconSP;
QPixmap* flagIconSPSel;
QPixmap* lockIcon;
QPixmap* tocIcon;
QPixmap* exitIconS;

QPixmap* undoIcon;
QPixmap* redoIcon;
QPixmap* undoIconS;
QPixmap* redoIconS;

QPixmap* speakerIcon;
QPixmap* buttondownIcon;
QPixmap* configureIcon;

QPixmap* multiDisplay;
QPixmap* previousPartIcon;
QPixmap* nextPartIcon;
QPixmap* blankRecord;
QPixmap* preIcon;
QPixmap* preIconOn;
QPixmap* mixerIn;
QPixmap* mixerOut;
QPixmap* recEchoIconOn;
QPixmap* recEchoIconOff;

QPixmap* muteIconOn;
QPixmap* muteIconOff;
QPixmap* muteIconOver;
QIcon* muteIconSet4;

QPixmap* soloIconOn;
QPixmap* soloIconOff;
QPixmap* soloIconOver;
QPixmap* transport_soloIconOn;
QPixmap* transport_soloIconOff;
QPixmap* transport_soloIconOver;
QPixmap* soloblksqIconOn;
QPixmap* soloblksqIconOff;
QPixmap* soloblksqIconOver;
QIcon* soloIconSet1;
QIcon* soloIconSet2;
QIcon* soloIconSet3;

QPixmap* transport_muteIconOn;
QPixmap* transport_muteIconOff;
QPixmap* transport_muteIconOver;
QIcon* muteIconSet3;

QPixmap* midiInIconOn;
QPixmap* midiInIconOff;
QPixmap* midiInIconOver;
QIcon* midiInIconSet3;

QPixmap* transport_recordIconOn;
QPixmap* transport_recordIconOff;
QPixmap* transport_recordIconOver;
QIcon* recordIconSet3;

QPixmap* transport_playIconOn;
QPixmap* transport_playIconOff;
QPixmap* transport_playIconOver;
QIcon* playIconSet3;

QPixmap* transport_playIconRightOn;
QPixmap* transport_playIconRightOff;
QPixmap* transport_playIconRightOver;
QIcon* playIconSetRight;

QPixmap* transport_stopIconLeftOn;
QPixmap* transport_stopIconLeftOff;
QPixmap* transport_stopIconLeftOver;
QIcon* stopIconSetLeft;

QPixmap* transport_startIconOn;
QPixmap* transport_startIconOff;
QPixmap* transport_startIconOver;
QIcon* startIconSet3;

QPixmap* transport_rewindIconOn;
QPixmap* transport_rewindIconOff;
QPixmap* transport_rewindIconOver;
QIcon* rewindIconSet3;

QPixmap* transport_forwardIconOn;
QPixmap* transport_forwardIconOff;
QPixmap* transport_forwardIconOver;
QIcon* forwardIconSet3;

QPixmap* transport_stopIconOn;
QPixmap* transport_stopIconOff;
QPixmap* transport_stopIconOver;
QIcon* stopIconSet3;

QPixmap* pointerIconOn;
QPixmap* pointerIconOff;
QPixmap* pointerIconOver;
QIcon* pointerIconSet3;

QPixmap* pencilIconOn;
QPixmap* pencilIconOff;
QPixmap* pencilIconOver;
QIcon* pencilIconSet3;

QPixmap* deleteIconOn;
QPixmap* deleteIconOff;
QPixmap* deleteIconOver;
QIcon* deleteIconSet3;

QPixmap* cutIconOn;
QPixmap* cutIconOff;
QPixmap* cutIconOver;
QIcon* cutIconSet3;

QIcon* note1IconSet3;

QPixmap* glueIconOn;
QPixmap* glueIconOff;
QPixmap* glueIconOver;
QIcon* glueIconSet3;

QIcon* quantIconSet3;

QPixmap* drawIconOn;
QPixmap* drawIconOff;
QPixmap* drawIconOver;
QIcon* drawIconSet3;

QPixmap* stretchIconOn;
QPixmap* stretchIconOff;
QPixmap* stretchIconOver;
QIcon* stretchIconSet3;

QPixmap* multiDisplayIconOn;
QPixmap* multiDisplayIconOff;
QPixmap* multiDisplayIconOver;
QIcon* multiDisplayIconSet3;

QPixmap* selectMultiIconOn;
QPixmap* selectMultiIconOff;
QPixmap* selectMultiIconOver;
QIcon* selectMultiIconSet3;

QPixmap* auditionIconOn;
QPixmap* auditionIconOff;
QPixmap* auditionIconOver;
QIcon* auditionIconSet3;

QPixmap* feedbackIconOn;
QPixmap* feedbackIconOff;
QPixmap* feedbackIconOver;
QIcon* feedbackIconSet3;

QPixmap* globalKeysIconOn;
QPixmap* globalKeysIconOff;
QPixmap* globalKeysIconOver;
QIcon* globalKeysIconSet3;

QPixmap* stepIconOn;
QPixmap* stepIconOff;
QPixmap* stepIconOver;
QIcon* stepIconSet3;

QPixmap* punchinIconOn;
QPixmap* punchinIconOff;
QPixmap* punchinIconOver;
QIcon* punchinIconSet3;

QPixmap* punchoutIconOn;
QPixmap* punchoutIconOff;
QPixmap* punchoutIconOver;
QIcon* punchoutIconSet3;

QPixmap* loopIconOn;
QPixmap* loopIconOff;
QPixmap* loopIconOver;
QIcon* loopIconSet3;

QPixmap* punchinVertIconOn;
QPixmap* punchinVertIconOff;
QPixmap* punchinVertIconOver;
QIcon* punchinVertIconSet3;

QPixmap* punchoutVertIconOn;
QPixmap* punchoutVertIconOff;
QPixmap* punchoutVertIconOver;
QIcon* punchoutVertIconSet3;

QPixmap* loopVertIconOn;
QPixmap* loopVertIconOff;
QPixmap* loopVertIconOver;
QIcon* loopVertIconSet3;

QPixmap* expandIconOn;
QPixmap* expandIconOff;
QPixmap* expandIconOver;
QIcon*   expandIconSet3;

QPixmap* vuIconOn;
QPixmap* vuIconOff;
QPixmap* vuIconOver;
QIcon*   vuIconSet3;

QPixmap* refreshIconOn;
QPixmap* refreshIconOff;
QPixmap* refreshIconOver;
QIcon*   refreshIconSet3;

QIcon* mixer_resizeIconSet3;
QIcon* mixer_inputIconSet3;
QIcon* mixer_outputIconSet3;
QIcon* mixer_powerIconSet3;
QIcon* mixer_recordIconSet3;
QIcon* mixer_muteIconSet3;
QIcon* mixer_soloIconSet3;
QIcon* mixer_stereoIconSet3;
QIcon* mixerIconSet3;
QIcon* pcloaderIconSet3;

QPixmap* enabled_OnIcon;
QPixmap* enabled_OffIcon;
QPixmap* enabled_OverIcon;
QIcon* enabledIconSet3;

QPixmap* up_arrow_OnIcon;
QPixmap* up_arrow_OffIcon;
QPixmap* up_arrow_OverIcon;
QIcon* up_arrowIconSet3;

QPixmap* down_arrow_OnIcon;
QPixmap* down_arrow_OffIcon;
QPixmap* down_arrow_OverIcon;
QIcon* down_arrowIconSet3;

QIcon* collapseIconSet3;

QPixmap* plus_OnIcon;
QPixmap* plus_OffIcon;
QPixmap* plus_OverIcon;
QIcon* plusIconSet3;

QPixmap* browse_OnIcon;
QPixmap* browse_OffIcon;
QPixmap* browse_OverIcon;
QIcon* browseIconSet3;

QPixmap* garbage_OnIcon;
QPixmap* garbage_OffIcon;
QPixmap* garbage_OverIcon;
QIcon* garbageIconSet3;

QPixmap* duplicate_OnIcon;
QPixmap* duplicate_OffIcon;
QPixmap* duplicate_OverIcon;
QIcon* duplicateIconSet3;

QPixmap* comment_OnIcon;
QPixmap* comment_OffIcon;
QPixmap* comment_OverIcon;
QIcon* commentIconSet3;

QPixmap* transpose_OnIcon;
QPixmap* transpose_OffIcon;
QPixmap* transpose_OverIcon;
QIcon* transposeIconSet3;

QPixmap* connect_OnIcon;
QPixmap* connect_OffIcon;
QPixmap* connect_OverIcon;
QIcon*   connectIconSet3;

QPixmap* input_OnIcon;
QPixmap* input_OffIcon;
QPixmap* input_OverIcon;
QIcon*   inputIconSet3;

QPixmap* load_OnIcon;
QPixmap* load_OffIcon;
QPixmap* load_OverIcon;
QIcon*   loadIconSet3;

QPixmap* pluginGUI_OnIcon;
QPixmap* pluginGUI_OffIcon;
QPixmap* pluginGUI_OverIcon;
QIcon*   pluginGUIIconSet3;

QPixmap* route_edit_OnIcon;
QPixmap* route_edit_OffIcon;
QPixmap* route_edit_OverIcon;
QIcon*   route_editIconSet3;

QPixmap* next_OnIcon;
QPixmap* next_OffIcon;
QPixmap* next_OverIcon;
QIcon*   nextIconSet3;

QPixmap* previous_OnIcon;
QPixmap* previous_OffIcon;
QPixmap* previous_OverIcon;
QIcon*   previousIconSet3;

QPixmap* dragMidiIcon;
QPixmap* dragAudioIcon;
QPixmap* dragInputIcon;
QPixmap* dragOutputIcon;
QPixmap* dragAuxIcon;
QPixmap* dragBussIcon;

QPixmap* mixer_record_OnIcon;
QPixmap* mixer_record_OffIcon;
QPixmap* mixer_record_OverIcon;
QPixmap* mixer_mute_OnIcon;
QPixmap* mixer_mute_OffIcon;
QPixmap* mixer_mute_OverIcon;
QPixmap* mixer_solo_OnIcon;
QPixmap* mixer_solo_OffIcon;
QPixmap* mixer_solo_OverIcon;
QPixmap* mixer_resize_OffIcon;
QPixmap* mixer_resize_OverIcon;
QPixmap* mixer_input_OffIcon;
QPixmap* mixer_input_OverIcon;
QPixmap* mixer_output_OffIcon;
QPixmap* mixer_output_OverIcon;
QPixmap* mixer_power_OnIcon;
QPixmap* mixer_power_OffIcon;
QPixmap* mixer_power_OverIcon;
QPixmap* mixer_blank_OffIcon;
QPixmap* mixer_stereo_OnIcon;
QPixmap* mixer_mono_OnIcon;

QPixmap* mixer_OnIcon;
QPixmap* mixer_OffIcon;
QPixmap* mixer_OverIcon;
QPixmap* pcloader_OnIcon;
QPixmap* pcloader_OffIcon;
QPixmap* pcloader_OverIcon;

QPixmap* speakerIconOn;
QPixmap* speakerIconOff;
QPixmap* speakerIconOver;
QIcon* speakerIconSet3;

QPixmap* globalArmIconOff;
QPixmap* globalArmIconOver;
QIcon*   globalArmIconSet3;

QPixmap* panicIconOff;
QPixmap* panicIconOver;
QIcon* panicIconSet3;

QPixmap* editmuteIcon;
QPixmap* editmuteSIcon;
QPixmap* panicIcon;
QPixmap* feedbackIcon;

QPixmap* arranger_record_on_Icon;
QPixmap* arranger_record_off_Icon;
QPixmap* arranger_record_over_Icon;
QPixmap* arranger_mute_on_Icon;
QPixmap* arranger_mute_off_Icon;
QPixmap* arranger_mute_over_Icon;
QPixmap* arranger_solo_on_Icon;
QPixmap* arranger_solo_off_Icon;
QPixmap* arranger_solo_over_Icon;
QPixmap* duplicatePCIcon;
QPixmap* garbagePCIcon;
QPixmap* upPCIcon;
QPixmap* downPCIcon;

QIcon* pianoIconSet;
QIcon* scoreIconSet;
QIcon* editcutIconSet;
QIcon* editmuteIconSet;
QIcon* editcopyIconSet;
QIcon* editpasteIconSet;
QIcon* editpaste2TrackIconSet;
QIcon* editpasteCloneIconSet;
QIcon* editpasteClone2TrackIconSet;

/* Not used - Orcan
QIcon* pianoIcon;
QIcon* editcutIcon;
QIcon* editcopyIcon;
QIcon* editpasteIcon;
QIcon* editpasteCloneIcon;
QIcon* editpaste2TrackIcon;
QIcon* editpasteClone2TrackIcon;
*/

QPixmap* redLedIcon;
QPixmap* darkRedLedIcon;
QPixmap* greendotIcon;
//QPixmap* darkgreendotIcon;
QPixmap* graydotIcon;
QPixmap* bluedotIcon;
QPixmap* offIcon;
QPixmap* blacksquareIcon;
QPixmap* blacksqcheckIcon;

QPixmap* addtrack_addmiditrackIcon;
QPixmap* addtrack_audiogroupIcon;
QPixmap* addtrack_audioinputIcon;
QPixmap* addtrack_audiooutputIcon;
QPixmap* addtrack_auxsendIcon;
QPixmap* addtrack_drumtrackIcon;
QPixmap* addtrack_wavetrackIcon;
QPixmap* edit_drummsIcon;
QPixmap* edit_listIcon;
QPixmap* remove_ctrlIcon;
QPixmap* edit_waveIcon;
QPixmap* edit_mastertrackIcon;
QPixmap* edit_pianorollIcon;
QPixmap* edit_scoreIcon;
QPixmap* edit_track_addIcon;
QPixmap* edit_track_delIcon;
QPixmap* mastertrack_graphicIcon;
QPixmap* mastertrack_listIcon;
QPixmap* midi_transformIcon;
QPixmap* midi_transposeIcon;
QPixmap* selectIcon;
QPixmap* selectMultiIcon;
QPixmap* select_allIcon;
QPixmap* select_all_parts_on_trackIcon;
QPixmap* select_deselect_allIcon;
QPixmap* select_inside_loopIcon;
QPixmap* select_invert_selectionIcon;
QPixmap* select_outside_loopIcon;

QPixmap* audio_bounce_to_fileIcon;
QPixmap* audio_bounce_to_trackIcon;
QPixmap* audio_restartaudioIcon;
QPixmap* automation_clear_dataIcon;
QPixmap* automation_mixerIcon;
QPixmap* automation_take_snapshotIcon;
QPixmap* edit_midiIcon;
QPixmap* midi_edit_instrumentIcon;
QPixmap* midi_init_instrIcon;
QPixmap* midi_inputpluginsIcon;
QPixmap* midi_inputplugins_midi_input_filterIcon;
QPixmap* midi_inputplugins_midi_input_transformIcon;
QPixmap* midi_inputplugins_random_rhythm_generatorIcon;
QPixmap* midi_inputplugins_remote_controlIcon;
QPixmap* midi_inputplugins_transposeIcon;
QPixmap* midi_local_offIcon;
QPixmap* midi_reset_instrIcon;
QPixmap* settings_appearance_settingsIcon;
QPixmap* settings_configureshortcutsIcon;
QPixmap* settings_follow_songIcon;
QPixmap* settings_globalsettingsIcon;
QPixmap* settings_midifileexportIcon;
QPixmap* settings_midiport_softsynthsIcon;
QPixmap* settings_midisyncIcon;
QPixmap* view_bigtime_windowIcon;
QPixmap* view_cliplistIcon;
QPixmap* view_markerIcon;
QPixmap* view_mixerIcon;
QPixmap* view_transport_windowIcon;

QPixmap* mixerIcon;
QPixmap* expandIcon;
QPixmap* monoIcon;
QPixmap* stereoIcon;
QPixmap* automationIcon;
QPixmap* portIcon;
QPixmap* automationDisabledIcon;
QPixmap* portDisabledIcon;
QPixmap* addTVIcon;
QPixmap* losIcon;
QPixmap* aboutLOSImage;
QPixmap* losLeftSideLogo;
QPixmap* reminder1_OnIcon;
QPixmap* reminder1_OffIcon;
QPixmap* reminder1_OverIcon;
QPixmap* reminder2_OnIcon;
QPixmap* reminder2_OffIcon;
QPixmap* reminder2_OverIcon;
QPixmap* reminder3_OnIcon;
QPixmap* reminder3_OffIcon;
QPixmap* reminder3_OverIcon;
QPixmap* record_track_OnIcon;
QPixmap* record_track_OffIcon;
QPixmap* record_track_OverIcon;
QPixmap* mute_track_OnIcon;
QPixmap* mute_track_OffIcon;
QPixmap* mute_track_OverIcon;
QPixmap* solo_track_OnIcon;
QPixmap* solo_track_OffIcon;
QPixmap* solo_track_OverIcon;
QPixmap* input_indicator_OnIcon;
QPixmap* input_indicator_OffIcon;
QPixmap* input_indicator_OverIcon;
QPixmap* automation_track_OffIcon;
QPixmap* automation_track_OverIcon;
QPixmap* instrument_track_OffIcon;
QPixmap* instrument_track_OverIcon;
QPixmap* instrument_track_ActiveIcon;

QPixmap* instrument_OnIcon;
QPixmap* instrument_OffIcon;
QPixmap* instrument_OverIcon;

QIcon* reminder1IconSet3;
QIcon* reminder2IconSet3;
QIcon* reminder3IconSet3;

QIcon* record_trackIconSet3;
QIcon* mute_trackIconSet3;
QIcon* solo_trackIconSet3;
QIcon* automation_trackIconSet3;
QIcon* instrument_trackIconSet3;
QIcon* instrumentIconSet3;
QIcon* input_indicatorIconSet3;

QPixmap* armAllIcon;
QPixmap* globalKeysIcon;

QIcon* globalIcon;
QIcon* projectIcon;
QIcon* userIcon;


QPixmap* sineIcon;
QPixmap* sawIcon;

//---------------------------------------------------------
//   initIcons
//---------------------------------------------------------

void initIcons()
{
    track_commentIcon = new LOSPixmap(track_comment_xpm, nullptr);
    crosshairIcon  = new LOSPixmap(":/images/crosshair.png", nullptr);
    pointerIcon  = new LOSPixmap(":/images/icons/select.png", nullptr);
    pencilIcon   = new LOSPixmap(":/images/icons/pencil.png", nullptr);
    pencilCursorIcon   = new LOSPixmap(":/images/icons/pencil_cursor.png", nullptr);
    deleteIcon   = new LOSPixmap(":/images/icons/eraser.png", nullptr);
    punchinIcon  = new LOSPixmap(":/images/icons/transport-punchin.png", nullptr);
    punchoutIcon = new LOSPixmap(":/images/icons/transport-punchout.png", nullptr);
    punchin1Icon = new LOSPixmap(":/images/icons/transport-punchin.png", nullptr);
    punchout1Icon = new LOSPixmap(":/images/icons/transport-punchout.png", nullptr);
    loopIcon     = new LOSPixmap(":/images/icons/transport-loop.png", nullptr);
    loop1Icon    = new LOSPixmap(":/images/icons/transport-loop.png", nullptr);
    playIcon     = new LOSPixmap(":/images/icons/transport-play.png", nullptr);
    auditionIcon     = new LOSPixmap(":/images/icons/audition.png", nullptr);

    recordIcon   = new LOSPixmap(":/images/icons/transport-record.png", nullptr);
    stopIcon     = new LOSPixmap(":/images/icons/transport-stop.png", nullptr);
    startIcon    = new LOSPixmap(":/images/icons/transport-rewind-end.png", nullptr);
    fforwardIcon = new LOSPixmap(":/images/icons/transport-ffwd.png", nullptr);
    frewindIcon  = new LOSPixmap(":/images/icons/transport-rewind.png", nullptr);
    dotIcon      = new LOSPixmap(":/images/icons/arranger_solo_on.png", nullptr);
    dothIcon     = new LOSPixmap(":/images/icons/arranger_solo_off.png", nullptr);
    dot1Icon     = new LOSPixmap(":/images/icons/arranger_solo_on.png", nullptr);
    noteIcon     = new LOSPixmap(note_xpm, nullptr);
    note1Icon    = new LOSPixmap(note1_xpm, nullptr);
    stickIcon    = new LOSPixmap(stick_xpm, nullptr);
    waveIcon     = new LOSPixmap(wave_xpm, nullptr);
    synthIcon    = new LOSPixmap(synth_xpm, nullptr);
    markIcon[0]  = new LOSPixmap(":/images/icons/cmark.png", nullptr);
    markIcon[1]  = new LOSPixmap(":/images/icons/lmark.png", nullptr);
    markIcon[2]  = new LOSPixmap(":/images/icons/rmark.png", nullptr);
    markIcon[3]  = new LOSPixmap(":/images/icons/emark.png", nullptr);
    cutIcon      = new LOSPixmap(":/images/icons/split.png", nullptr);
    steprecIcon  = new LOSPixmap(":/images/icons/step_by_step.png", nullptr);
    glueIcon     = new LOSPixmap(":/images/icons/join_tracks.png", nullptr);
    drawIcon     = new LOSPixmap(":/images/icons/line-tool.png", nullptr);
    quantIcon    = new LOSPixmap(quant_xpm, nullptr);
    saveIcon     = new LOSPixmap(filesave_xpm, nullptr);
    saveasIcon     = new LOSPixmap(filesaveas_xpm, nullptr);
    printIcon    = new LOSPixmap(fileprint_xpm, nullptr);//"document-print");
    openIcon     = new LOSPixmap(fileopen_xpm, nullptr);//"document-open");
    saveIconS     = new LOSPixmap(filesaveS_xpm, nullptr);//"document-save");
    saveasIconS     = new LOSPixmap(filesaveasS_xpm, nullptr);//"document-save-as");
    printIconS    = new LOSPixmap(fileprintS_xpm, nullptr);//"document-print");
    openIconS     = new LOSPixmap(fileopenS_xpm, nullptr);//"document-open");
    masterIcon   = new LOSPixmap(master_xpm, nullptr);//"mixer-master");
    filenewIcon  = new LOSPixmap(filenew_xpm, nullptr);//"document-new");
    filenewIconS  = new LOSPixmap(filenewS_xpm, nullptr);//"document-new");
    homeIcon     = new LOSPixmap(home_xpm, nullptr);//"user-home");
    backIcon     = new LOSPixmap(back_xpm, nullptr);//"go-previous");
    forwardIcon  = new LOSPixmap(forward_xpm, nullptr);//"go-next");
    muteIcon     = new LOSPixmap(editmuteS_xpm, nullptr);//"audio-volume-muted");
    upIcon       = new LOSPixmap(up_xpm, nullptr);//"go-up");
    downIcon     = new LOSPixmap(down_xpm, nullptr);//"go-down");
    boldIcon     = new LOSPixmap(bold_xpm, nullptr);//"format-text-bold");
    italicIcon     = new LOSPixmap(italic_xpm, nullptr);//"format-text-italic");
    underlinedIcon = new LOSPixmap(underlined_xpm, nullptr);//"format-text-underline");
    gvIcon     = new LOSPixmap(gv_xpm, nullptr);
    midiinIcon = new LOSPixmap(midiin_xpm, nullptr);
    sysexIcon   = new LOSPixmap(sysex_xpm, nullptr);
    ctrlIcon    = new LOSPixmap(ctrl_xpm, nullptr);
    metaIcon    = new LOSPixmap(meta_xpm, nullptr);
    pitchIcon   = new LOSPixmap(pitch_xpm, nullptr);
    cafterIcon  = new LOSPixmap(cafter_xpm, nullptr);
    pafterIcon  = new LOSPixmap(pafter_xpm, nullptr);
    flagIcon    = new LOSPixmap(flag_xpm, nullptr);
    flagIconS   = new LOSPixmap(flagS_xpm, nullptr);
    flagIconSP  = new LOSPixmap(":/images/flagSP.png", nullptr);//ProgramChange Flag
    flagIconSPSel  = new LOSPixmap(":/images/flagSP-select.png", nullptr);//ProgramChange Flag
    upPCIcon 	  = new LOSPixmap(":/images/icons/up.png", nullptr);//ProgramChange Flag
    downPCIcon   = new LOSPixmap(":/images/icons/down.png", nullptr);//ProgramChange Flag
    garbagePCIcon = new LOSPixmap(":/images/icons/garbage.png", nullptr);//ProgramChange Flag
    duplicatePCIcon = new LOSPixmap(":/images/icons/duplicate.png", nullptr);//ProgramChange Flag
    arranger_record_on_Icon = new LOSPixmap(":/images/icons/arranger_record_on.png", nullptr);//ProgramChange Flag
    arranger_record_off_Icon = new LOSPixmap(":/images/icons/arranger_record_off.png", nullptr);//ProgramChange Flag
    arranger_record_over_Icon = new LOSPixmap(":/images/icons/arranger_record_over.png", nullptr);//ProgramChange Flag
    arranger_mute_on_Icon = new LOSPixmap(":/images/icons/arranger_mute_on.png", nullptr);//ProgramChange Flag
    arranger_mute_off_Icon = new LOSPixmap(":/images/icons/arranger_mute_off.png", nullptr);//ProgramChange Flag
    arranger_mute_over_Icon = new LOSPixmap(":/images/icons/arranger_mute_over.png", nullptr);//ProgramChange Flag
    arranger_solo_on_Icon = new LOSPixmap(":/images/icons/arranger_solo_on.png", nullptr);//ProgramChange Flag
    arranger_solo_off_Icon = new LOSPixmap(":/images/icons/arranger_solo_off.png", nullptr);//ProgramChange Flag
    arranger_solo_over_Icon = new LOSPixmap(":/images/icons/arranger_solo_over.png", nullptr);//ProgramChange Flag
    duplicatePCIcon = new LOSPixmap(":/images/icons/duplicate.png", nullptr);//ProgramChange Flag
    lockIcon    = new LOSPixmap(lock_xpm, nullptr);
    tocIcon     = new LOSPixmap(toc_xpm, nullptr);
    exitIconS   = new LOSPixmap(exitS_xpm, nullptr);//"application-exit");

    undoIcon     = new LOSPixmap(undo_xpm, nullptr);//"edit-undo");
    redoIcon     = new LOSPixmap(redo_xpm, nullptr);//"edit-redo");
    undoIconS    = new LOSPixmap(undoS_xpm, nullptr);//"edit-undo");
    redoIconS    = new LOSPixmap(redoS_xpm, nullptr);//"edit-redo");

    speakerIcon    = new LOSPixmap(":/images/icons/speaker.png", nullptr);
    buttondownIcon = new LOSPixmap(buttondown_xpm, nullptr);//"arrow-down");
    configureIcon  = new LOSPixmap(configure_xpm, nullptr);
    armAllIcon = new LOSPixmap(":/images/icons/globe-edit.png", nullptr);
    globalKeysIcon = new LOSPixmap(":/images/icons/globe.png", nullptr);

    editmuteIcon  = new LOSPixmap(":/images/icons/mute-all.png", nullptr);
    editmuteSIcon = new LOSPixmap(editmuteS_xpm, nullptr);
    panicIcon  = new LOSPixmap(":/images/icons/transport-panic.png", nullptr);
    feedbackIcon  = new LOSPixmap(":/images/icons/feedback.png", nullptr);

    editcutIconSet       = new LOSIcon(editcutS_xpm, nullptr);//"edit-cut"); // ddskrjo
    editcopyIconSet      = new LOSIcon(editcopyS_xpm, nullptr);//"edit-copy");
    editpasteIconSet     = new LOSIcon(editpasteS_xpm, nullptr);//"edit-paste");
    editmuteIconSet      = new LOSIcon(editmuteS_xpm, nullptr);//"audio-volume-muted");
    editpaste2TrackIconSet = new LOSIcon(editpaste2trackS_xpm, nullptr);
    editpasteCloneIconSet  = new LOSIcon(editpastecloneS_xpm, nullptr);
    editpasteClone2TrackIconSet = new LOSIcon(editpasteclone2trackS_xpm, nullptr); // ..
    exitIcon             = new LOSPixmap(":/images/icons/mixer-exit.png", nullptr);//"application-exit");
    exit1Icon            = new LOSPixmap(":/images/icons/mixer-exit_on.png", nullptr);//"application-exit");

    // 2 lines odd code
    newmuteIcon          = new LOSPixmap(newmutebutton_xpm, nullptr);
    soloIcon             = new LOSPixmap(solobutton_xpm, nullptr);

    multiDisplay         = new LOSPixmap(":/images/icons/multi_display.png", nullptr);
    previousPartIcon     = new LOSPixmap(":/images/icons/previous_part.png", nullptr);
    nextPartIcon         = new LOSPixmap(":/images/icons/next_part.png", nullptr);
    blankRecord          = new LOSPixmap(":/images/icons/blank_record.png", nullptr);
    preIcon        	   = new LOSPixmap(":/images/icons/mixer-pre.png", nullptr);
    preIconOn        	   = new LOSPixmap(":/images/icons/mixer-pre_on.png", nullptr);
    mixerIn        	   = new LOSPixmap(":/images/icons/mixer-in.png", nullptr);
    mixerOut        	   = new LOSPixmap(":/images/icons/mixer-out.png", nullptr);
    recEchoIconOn        = new LOSPixmap(":/images/icons/mixer-record.png", nullptr);
    recEchoIconOff       = new LOSPixmap(":/images/icons/mixer-record.png", nullptr);
    muteIconOn           = new LOSPixmap(":/images/icons/mixer-mute_on.png", nullptr);
    muteIconOff          = new LOSPixmap(":/images/icons/mixer-mute.png", nullptr);
    muteIconOver         = new LOSPixmap(":/images/icons/mixer-mute_over.png", nullptr);
    soloIconOn           = new LOSPixmap(":/images/icons/mixer-solo_on.png", nullptr);
    soloIconOff          = new LOSPixmap(":/images/icons/mixer-solo.png", nullptr);
    soloIconOver         = new LOSPixmap(":/images/icons/mixer-solo_over.png", nullptr);
    soloblksqIconOn      = new LOSPixmap(":/images/icons/mixer-solo_on.png", nullptr);
    soloblksqIconOff     = new LOSPixmap(":/images/icons/mixer-solo.png", nullptr);
    soloblksqIconOver    = new LOSPixmap(":/images/icons/mixer-solo_over.png", nullptr);
    transport_soloIconOn = new LOSPixmap(":/images/icons/transport-solo_new_on.png", nullptr);
    transport_soloIconOff= new LOSPixmap(":/images/icons/transport-solo_new_off.png", nullptr);
    transport_soloIconOver= new LOSPixmap(":/images/icons/transport-solo_new_over.png", nullptr);
    soloIconSet1         = new QIcon();
    soloIconSet2         = new QIcon();
    soloIconSet3         = new QIcon();
    soloIconSet1->addPixmap(*soloIconOn, QIcon::Normal, QIcon::On);
    soloIconSet1->addPixmap(*soloIconOff, QIcon::Normal, QIcon::Off);
    soloIconSet1->addPixmap(*soloIconOver, QIcon::Active);
    soloIconSet2->addPixmap(*soloblksqIconOn, QIcon::Normal, QIcon::On);
    soloIconSet2->addPixmap(*soloblksqIconOff, QIcon::Normal, QIcon::Off);
    soloIconSet2->addPixmap(*soloblksqIconOver, QIcon::Active);
    soloIconSet3->addPixmap(*transport_soloIconOn, QIcon::Normal, QIcon::On);
    soloIconSet3->addPixmap(*transport_soloIconOff, QIcon::Normal, QIcon::Off);
    soloIconSet3->addPixmap(*transport_soloIconOver, QIcon::Active);

    transport_muteIconOn = new LOSPixmap(":/images/icons/transport-mute_new_on.png", nullptr);
    transport_muteIconOff= new LOSPixmap(":/images/icons/transport-mute_new_off.png", nullptr);
    transport_muteIconOver= new LOSPixmap(":/images/icons/transport-mute_new_over.png", nullptr);
    muteIconSet3         = new QIcon();
    muteIconSet3->addPixmap(*transport_muteIconOn, QIcon::Normal, QIcon::On);
    muteIconSet3->addPixmap(*transport_muteIconOff, QIcon::Normal, QIcon::Off);
    muteIconSet3->addPixmap(*transport_muteIconOver, QIcon::Active);

    midiInIconOn = new LOSPixmap(":/images/icons/midiin_new_on.png", nullptr);
    midiInIconOff= new LOSPixmap(":/images/icons/midiin_new_off.png", nullptr);
    midiInIconOver= new LOSPixmap(":/images/icons/midiin_new_over.png", nullptr);
    midiInIconSet3         = new QIcon();
    midiInIconSet3->addPixmap(*midiInIconOn, QIcon::Normal, QIcon::On);
    midiInIconSet3->addPixmap(*midiInIconOff, QIcon::Normal, QIcon::Off);
    midiInIconSet3->addPixmap(*midiInIconOver, QIcon::Active);
    muteIconSet4         = new QIcon();
    transport_recordIconOn = new LOSPixmap(":/images/icons/transport-record_new_on.png", nullptr);
    transport_recordIconOff = new LOSPixmap(":/images/icons/transport-record_new_off.png", nullptr);
    transport_recordIconOver = new LOSPixmap(":/images/icons/transport-record_new_over.png", nullptr);
    recordIconSet3         = new QIcon();
    recordIconSet3->addPixmap(*transport_recordIconOn, QIcon::Normal, QIcon::On);
    recordIconSet3->addPixmap(*transport_recordIconOff, QIcon::Normal, QIcon::Off);
    recordIconSet3->addPixmap(*transport_recordIconOver, QIcon::Active);

    transport_playIconOn = new LOSPixmap(":/images/icons/transport-play_new_on.png", nullptr);
    transport_playIconOff= new LOSPixmap(":/images/icons/transport-play_new_off.png", nullptr);
    transport_playIconOver= new LOSPixmap(":/images/icons/transport-play_new_over.png", nullptr);
    playIconSet3         = new QIcon();
    playIconSet3->addPixmap(*transport_playIconOn, QIcon::Normal, QIcon::On);
    playIconSet3->addPixmap(*transport_playIconOff, QIcon::Normal, QIcon::Off);
    playIconSet3->addPixmap(*transport_playIconOver, QIcon::Active);

    transport_playIconRightOn = new LOSPixmap(":/images/icons/transport-play_new_end_on.png", nullptr);
    transport_playIconRightOff= new LOSPixmap(":/images/icons/transport-play_new_end_off.png", nullptr);
    transport_playIconRightOver= new LOSPixmap(":/images/icons/transport-play_new_end_over.png", nullptr);
    playIconSetRight         = new QIcon();
    playIconSetRight->addPixmap(*transport_playIconRightOn, QIcon::Normal, QIcon::On);
    playIconSetRight->addPixmap(*transport_playIconRightOff, QIcon::Normal, QIcon::Off);
    playIconSetRight->addPixmap(*transport_playIconRightOver, QIcon::Active);

    transport_stopIconLeftOn = new LOSPixmap(":/images/icons/transport-stop_new_end_on.png", nullptr);
    transport_stopIconLeftOff= new LOSPixmap(":/images/icons/transport-stop_new_end_off.png", nullptr);
    transport_stopIconLeftOver= new LOSPixmap(":/images/icons/transport-stop_new_end_over.png", nullptr);
    stopIconSetLeft         = new QIcon();
    stopIconSetLeft->addPixmap(*transport_stopIconLeftOn, QIcon::Normal, QIcon::On);
    stopIconSetLeft->addPixmap(*transport_stopIconLeftOff, QIcon::Normal, QIcon::Off);
    stopIconSetLeft->addPixmap(*transport_stopIconLeftOver, QIcon::Active);

    transport_startIconOff= new LOSPixmap(":/images/icons/transport-rewind-end_new_off.png", nullptr);
    transport_startIconOver= new LOSPixmap(":/images/icons/transport-rewind-end_new_over.png", nullptr);
    startIconSet3         = new QIcon();
    startIconSet3->addPixmap(*transport_startIconOff, QIcon::Normal, QIcon::Off);
    startIconSet3->addPixmap(*transport_startIconOver, QIcon::Active);

    transport_rewindIconOff= new LOSPixmap(":/images/icons/transport-rewind_new_off.png", nullptr);
    transport_rewindIconOver= new LOSPixmap(":/images/icons/transport-rewind_new_over.png", nullptr);
    rewindIconSet3         = new QIcon();
    rewindIconSet3->addPixmap(*transport_rewindIconOff, QIcon::Normal, QIcon::Off);
    rewindIconSet3->addPixmap(*transport_rewindIconOver, QIcon::Active);

    transport_forwardIconOff= new LOSPixmap(":/images/icons/transport-ffwd_new_off.png", nullptr);
    transport_forwardIconOver= new LOSPixmap(":/images/icons/transport-ffwd_new_over.png", nullptr);
    forwardIconSet3         = new QIcon();
    forwardIconSet3->addPixmap(*transport_forwardIconOff, QIcon::Normal, QIcon::Off);
    forwardIconSet3->addPixmap(*transport_forwardIconOver, QIcon::Active);

    transport_stopIconOn = new LOSPixmap(":/images/icons/transport-stop_new_on.png", nullptr);
    transport_stopIconOff= new LOSPixmap(":/images/icons/transport-stop_new_off.png", nullptr);
    transport_stopIconOver= new LOSPixmap(":/images/icons/transport-stop_new_over.png", nullptr);
    stopIconSet3         = new QIcon();
    stopIconSet3->addPixmap(*transport_stopIconOn, QIcon::Normal, QIcon::On);
    stopIconSet3->addPixmap(*transport_stopIconOff, QIcon::Normal, QIcon::Off);
    stopIconSet3->addPixmap(*transport_stopIconOver, QIcon::Active);

    pointerIconOn = new LOSPixmap(":/images/icons/pointer_new_on.png", nullptr);
    pointerIconOff= new LOSPixmap(":/images/icons/pointer_new_off.png", nullptr);
    pointerIconOver= new LOSPixmap(":/images/icons/pointer_new_over.png", nullptr);
    pointerIconSet3         = new QIcon();
    pointerIconSet3->addPixmap(*pointerIconOn, QIcon::Normal, QIcon::On);
    pointerIconSet3->addPixmap(*pointerIconOff, QIcon::Normal, QIcon::Off);
    pointerIconSet3->addPixmap(*pointerIconOver, QIcon::Active);

    deleteIconOn = new LOSPixmap(":/images/icons/eraser_new_on.png", nullptr);
    deleteIconOff= new LOSPixmap(":/images/icons/eraser_new_off.png", nullptr);
    deleteIconOver= new LOSPixmap(":/images/icons/eraser_new_over.png", nullptr);
    deleteIconSet3         = new QIcon();
    deleteIconSet3->addPixmap(*deleteIconOn, QIcon::Normal, QIcon::On);
    deleteIconSet3->addPixmap(*deleteIconOff, QIcon::Normal, QIcon::Off);
    deleteIconSet3->addPixmap(*deleteIconOver, QIcon::Active);

    pencilIconOn = new LOSPixmap(":/images/icons/pencil_new_on.png", nullptr);
    pencilIconOff= new LOSPixmap(":/images/icons/pencil_new_off.png", nullptr);
    pencilIconOver= new LOSPixmap(":/images/icons/pencil_new_over.png", nullptr);
    pencilIconSet3         = new QIcon();
    pencilIconSet3->addPixmap(*pencilIconOn, QIcon::Normal, QIcon::On);
    pencilIconSet3->addPixmap(*pencilIconOff, QIcon::Normal, QIcon::Off);
    pencilIconSet3->addPixmap(*pencilIconOver, QIcon::Active);

    cutIconOn = new LOSPixmap(":/images/icons/split_new_on.png", nullptr);
    cutIconOff= new LOSPixmap(":/images/icons/split_new_off.png", nullptr);
    cutIconOver= new LOSPixmap(":/images/icons/split_new_over.png", nullptr);
    cutIconSet3         = new QIcon();
    cutIconSet3->addPixmap(*cutIconOn, QIcon::Normal, QIcon::On);
    cutIconSet3->addPixmap(*cutIconOff, QIcon::Normal, QIcon::Off);
    cutIconSet3->addPixmap(*cutIconOver, QIcon::Active);

    note1IconSet3         = new QIcon();

    glueIconOn = new LOSPixmap(":/images/icons/join_new_on.png", nullptr);
    glueIconOff= new LOSPixmap(":/images/icons/join_new_off.png", nullptr);
    glueIconOver= new LOSPixmap(":/images/icons/join_new_over.png", nullptr);
    glueIconSet3         = new QIcon();
    glueIconSet3->addPixmap(*glueIconOn, QIcon::Normal, QIcon::On);
    glueIconSet3->addPixmap(*glueIconOff, QIcon::Normal, QIcon::Off);
    glueIconSet3->addPixmap(*glueIconOver, QIcon::Active);

    quantIconSet3         = new QIcon();

    drawIconOn = new LOSPixmap(":/images/icons/linetool_new_on.png", nullptr);
    drawIconOff= new LOSPixmap(":/images/icons/linetool_new_off.png", nullptr);
    drawIconOver= new LOSPixmap(":/images/icons/linetool_new_over.png", nullptr);
    drawIconSet3         = new QIcon();
    drawIconSet3->addPixmap(*drawIconOn, QIcon::Normal, QIcon::On);
    drawIconSet3->addPixmap(*drawIconOff, QIcon::Normal, QIcon::Off);
    drawIconSet3->addPixmap(*drawIconOver, QIcon::Active);

    stretchIconOn = new LOSPixmap(":/images/icons/stretchtool_new_on.png", nullptr);
    stretchIconOff= new LOSPixmap(":/images/icons/stretchtool_new_off.png", nullptr);
    stretchIconOver= new LOSPixmap(":/images/icons/stretchtool_new_over.png", nullptr);
    stretchIconSet3         = new QIcon();
    stretchIconSet3->addPixmap(*stretchIconOn, QIcon::Normal, QIcon::On);
    stretchIconSet3->addPixmap(*stretchIconOff, QIcon::Normal, QIcon::Off);
    stretchIconSet3->addPixmap(*stretchIconOver, QIcon::Active);

    multiDisplayIconOn = new LOSPixmap(":/images/icons/epicParts_new_on.png", nullptr);
    multiDisplayIconOff= new LOSPixmap(":/images/icons/epicParts_new_off.png", nullptr);
    multiDisplayIconOver= new LOSPixmap(":/images/icons/epicParts_new_over.png", nullptr);
    multiDisplayIconSet3         = new QIcon();
    multiDisplayIconSet3->addPixmap(*multiDisplayIconOn, QIcon::Normal, QIcon::On);
    multiDisplayIconSet3->addPixmap(*multiDisplayIconOff, QIcon::Normal, QIcon::Off);
    multiDisplayIconSet3->addPixmap(*multiDisplayIconOver, QIcon::Active);

    selectMultiIconOn = new LOSPixmap(":/images/icons/epicSelect_new_on.png", nullptr);
    selectMultiIconOff= new LOSPixmap(":/images/icons/epicSelect_new_off.png", nullptr);
    selectMultiIconOver= new LOSPixmap(":/images/icons/epicSelect_new_over.png", nullptr);
    selectMultiIconSet3         = new QIcon();
    selectMultiIconSet3->addPixmap(*selectMultiIconOn, QIcon::Normal, QIcon::On);
    selectMultiIconSet3->addPixmap(*selectMultiIconOff, QIcon::Normal, QIcon::Off);
    selectMultiIconSet3->addPixmap(*selectMultiIconOver, QIcon::Active);

    auditionIconOn = new LOSPixmap(":/images/icons/audition_new_on.png", nullptr);
    auditionIconOff= new LOSPixmap(":/images/icons/audition_new_off.png", nullptr);
    auditionIconOver= new LOSPixmap(":/images/icons/audition_new_over.png", nullptr);
    auditionIconSet3         = new QIcon();
    auditionIconSet3->addPixmap(*auditionIconOn, QIcon::Normal, QIcon::On);
    auditionIconSet3->addPixmap(*auditionIconOff, QIcon::Normal, QIcon::Off);
    auditionIconSet3->addPixmap(*auditionIconOver, QIcon::Active);

    feedbackIconOn = new LOSPixmap(":/images/icons/feedback_new_on.png", nullptr);
    feedbackIconOff= new LOSPixmap(":/images/icons/feedback_new_off.png", nullptr);
    feedbackIconOver= new LOSPixmap(":/images/icons/feedback_new_over.png", nullptr);
    feedbackIconSet3         = new QIcon();
    feedbackIconSet3->addPixmap(*feedbackIconOn, QIcon::Normal, QIcon::On);
    feedbackIconSet3->addPixmap(*feedbackIconOff, QIcon::Normal, QIcon::Off);
    feedbackIconSet3->addPixmap(*feedbackIconOver, QIcon::Active);

    globalKeysIconOn = new LOSPixmap(":/images/icons/epicDraw_new_on.png", nullptr);
    globalKeysIconOff= new LOSPixmap(":/images/icons/epicDraw_new_off.png", nullptr);
    globalKeysIconOver= new LOSPixmap(":/images/icons/epicDraw_new_over.png", nullptr);
    globalKeysIconSet3         = new QIcon();
    globalKeysIconSet3->addPixmap(*globalKeysIconOn, QIcon::Normal, QIcon::On);
    globalKeysIconSet3->addPixmap(*globalKeysIconOff, QIcon::Normal, QIcon::Off);
    globalKeysIconSet3->addPixmap(*globalKeysIconOver, QIcon::Active);

    stepIconOn = new LOSPixmap(":/images/icons/step_new_on.png", nullptr);
    stepIconOff= new LOSPixmap(":/images/icons/step_new_off.png", nullptr);
    stepIconOver= new LOSPixmap(":/images/icons/step_new_over.png", nullptr);
    stepIconSet3         = new QIcon();
    stepIconSet3->addPixmap(*stepIconOn, QIcon::Normal, QIcon::On);
    stepIconSet3->addPixmap(*stepIconOff, QIcon::Normal, QIcon::Off);
    stepIconSet3->addPixmap(*stepIconOver, QIcon::Active);

    punchinIconOn = new LOSPixmap(":/images/icons/left_new_on.png", nullptr);
    punchinIconOff= new LOSPixmap(":/images/icons/left_new_off.png", nullptr);
    punchinIconOver= new LOSPixmap(":/images/icons/left_new_over.png", nullptr);
    punchinIconSet3         = new QIcon();
    punchinIconSet3->addPixmap(*punchinIconOn, QIcon::Normal, QIcon::On);
    punchinIconSet3->addPixmap(*punchinIconOff, QIcon::Normal, QIcon::Off);
    punchinIconSet3->addPixmap(*punchinIconOver, QIcon::Active);

    punchoutIconOn = new LOSPixmap(":/images/icons/right_new_on.png", nullptr);
    punchoutIconOff= new LOSPixmap(":/images/icons/right_new_off.png", nullptr);
    punchoutIconOver= new LOSPixmap(":/images/icons/right_new_over.png", nullptr);
    punchoutIconSet3         = new QIcon();
    punchoutIconSet3->addPixmap(*punchoutIconOn, QIcon::Normal, QIcon::On);
    punchoutIconSet3->addPixmap(*punchoutIconOff, QIcon::Normal, QIcon::Off);
    punchoutIconSet3->addPixmap(*punchoutIconOver, QIcon::Active);

    loopIconOn = new LOSPixmap(":/images/icons/loop_new_on.png", nullptr);
    loopIconOff= new LOSPixmap(":/images/icons/loop_new_off.png", nullptr);
    loopIconOver= new LOSPixmap(":/images/icons/loop_new_over.png", nullptr);
    loopIconSet3         = new QIcon();
    loopIconSet3->addPixmap(*loopIconOn, QIcon::Normal, QIcon::On);
    loopIconSet3->addPixmap(*loopIconOff, QIcon::Normal, QIcon::Off);
    loopIconSet3->addPixmap(*loopIconOver, QIcon::Active);

    punchinVertIconOn = new LOSPixmap(":/images/icons/left_new_on_v.png", nullptr);
    punchinVertIconOff= new LOSPixmap(":/images/icons/left_new_off_v.png", nullptr);
    punchinVertIconOver= new LOSPixmap(":/images/icons/left_new_over_v.png", nullptr);
    punchinVertIconSet3         = new QIcon();
    punchinVertIconSet3->addPixmap(*punchinVertIconOn, QIcon::Normal, QIcon::On);
    punchinVertIconSet3->addPixmap(*punchinVertIconOff, QIcon::Normal, QIcon::Off);
    punchinVertIconSet3->addPixmap(*punchinVertIconOver, QIcon::Active);

    punchoutVertIconOn = new LOSPixmap(":/images/icons/right_new_on_v.png", nullptr);
    punchoutVertIconOff= new LOSPixmap(":/images/icons/right_new_off_v.png", nullptr);
    punchoutVertIconOver= new LOSPixmap(":/images/icons/right_new_over_v.png", nullptr);
    punchoutVertIconSet3         = new QIcon();
    punchoutVertIconSet3->addPixmap(*punchoutVertIconOn, QIcon::Normal, QIcon::On);
    punchoutVertIconSet3->addPixmap(*punchoutVertIconOff, QIcon::Normal, QIcon::Off);
    punchoutVertIconSet3->addPixmap(*punchoutVertIconOver, QIcon::Active);

    loopVertIconOn = new LOSPixmap(":/images/icons/loop_new_on_v.png", nullptr);
    loopVertIconOff= new LOSPixmap(":/images/icons/loop_new_off_v.png", nullptr);
    loopVertIconOver= new LOSPixmap(":/images/icons/loop_new_over_v.png", nullptr);
    loopVertIconSet3         = new QIcon();
    loopVertIconSet3->addPixmap(*loopVertIconOn, QIcon::Normal, QIcon::On);
    loopVertIconSet3->addPixmap(*loopVertIconOff, QIcon::Normal, QIcon::Off);
    loopVertIconSet3->addPixmap(*loopVertIconOver, QIcon::Active);

    expandIconOn = new LOSPixmap(":/images/expand_new_on.png", nullptr);
    expandIconOff= new LOSPixmap(":/images/expand_new_off.png", nullptr);
    expandIconOver= new LOSPixmap(":/images/expand_new_over.png", nullptr);
    expandIconSet3         = new QIcon();
    expandIconSet3->addPixmap(*expandIconOn, QIcon::Normal, QIcon::On);
    expandIconSet3->addPixmap(*expandIconOff, QIcon::Normal, QIcon::Off);
    expandIconSet3->addPixmap(*expandIconOver, QIcon::Active);

    vuIconOn = new LOSPixmap(":/images/vu_new_on.png", nullptr);
    vuIconOff= new LOSPixmap(":/images/vu_new_off.png", nullptr);
    vuIconOver= new LOSPixmap(":/images/vu_new_over.png", nullptr);
    vuIconSet3         = new QIcon();
    vuIconSet3->addPixmap(*vuIconOn, QIcon::Normal, QIcon::On);
    vuIconSet3->addPixmap(*vuIconOff, QIcon::Normal, QIcon::Off);
    vuIconSet3->addPixmap(*vuIconOver, QIcon::Active);

    refreshIconOn = new LOSPixmap(":/images/refresh_new_on.png", nullptr);
    refreshIconOff= new LOSPixmap(":/images/refresh_new_off.png", nullptr);
    refreshIconOver= new LOSPixmap(":/images/refresh_new_over.png", nullptr);
    refreshIconSet3         = new QIcon();
    refreshIconSet3->addPixmap(*refreshIconOff, QIcon::Normal, QIcon::Off);
    refreshIconSet3->addPixmap(*refreshIconOver, QIcon::Active);

    speakerIconOn = new LOSPixmap(":/images/icons/speaker_new_on.png", nullptr);
    speakerIconOff= new LOSPixmap(":/images/icons/speaker_new_off.png", nullptr);
    speakerIconOver= new LOSPixmap(":/images/icons/speaker_new_over.png", nullptr);
    speakerIconSet3         = new QIcon();
    speakerIconSet3->addPixmap(*speakerIconOn, QIcon::Normal, QIcon::On);
    speakerIconSet3->addPixmap(*speakerIconOff, QIcon::Normal, QIcon::Off);
    speakerIconSet3->addPixmap(*speakerIconOver, QIcon::Active);

    globalArmIconOver = new LOSPixmap(":/images/icons/epicRecord_new_over.png", nullptr);
    globalArmIconOff= new LOSPixmap(":/images/icons/epicRecord_new_off.png", nullptr);
    globalArmIconSet3         = new QIcon();
    globalArmIconSet3->addPixmap(*globalArmIconOff, QIcon::Normal, QIcon::Off);
    globalArmIconSet3->addPixmap(*globalArmIconOver, QIcon::Active);

    panicIconOver = new LOSPixmap(":/images/icons/panic_new_over.png", nullptr);
    panicIconOff= new LOSPixmap(":/images/icons/panic_new_off.png", nullptr);
    panicIconSet3         = new QIcon();
    panicIconSet3->addPixmap(*panicIconOff, QIcon::Normal, QIcon::Off);
    panicIconSet3->addPixmap(*panicIconOver, QIcon::Active);

    mixer_resize_OffIcon= new LOSPixmap(":/images/icons/mixer_resize_new_off.png", nullptr);
    mixer_resize_OverIcon= new LOSPixmap(":/images/icons/mixer_resize_new_over.png", nullptr);
    mixer_resizeIconSet3         = new QIcon();
    mixer_resizeIconSet3->addPixmap(*mixer_resize_OffIcon, QIcon::Normal, QIcon::Off);
    mixer_resizeIconSet3->addPixmap(*mixer_resize_OverIcon, QIcon::Active);

    mixer_record_OnIcon = new LOSPixmap(":/images/icons/mixer_record_new_on.png", nullptr);
    mixer_record_OffIcon= new LOSPixmap(":/images/icons/mixer_record_new_off.png", nullptr);
    mixer_record_OverIcon= new LOSPixmap(":/images/icons/mixer_record_new_over.png", nullptr);
    mixer_recordIconSet3         = new QIcon();
    mixer_recordIconSet3->addPixmap(*mixer_record_OnIcon, QIcon::Normal, QIcon::On);
    mixer_recordIconSet3->addPixmap(*mixer_record_OffIcon, QIcon::Normal, QIcon::Off);
    mixer_recordIconSet3->addPixmap(*mixer_record_OverIcon, QIcon::Active);

    mixer_mute_OnIcon = new LOSPixmap(":/images/icons/mixer_mute_new_on.png", nullptr);
    mixer_mute_OffIcon= new LOSPixmap(":/images/icons/mixer_mute_new_off.png", nullptr);
    mixer_mute_OverIcon= new LOSPixmap(":/images/icons/mixer_mute_new_over.png", nullptr);
    mixer_muteIconSet3         = new QIcon();
    mixer_muteIconSet3->addPixmap(*mixer_mute_OnIcon, QIcon::Normal, QIcon::On);
    mixer_muteIconSet3->addPixmap(*mixer_mute_OffIcon, QIcon::Normal, QIcon::Off);
    mixer_muteIconSet3->addPixmap(*mixer_mute_OverIcon, QIcon::Active);

    mixer_solo_OnIcon = new LOSPixmap(":/images/icons/mixer_solo_new_on.png", nullptr);
    mixer_solo_OffIcon= new LOSPixmap(":/images/icons/mixer_solo_new_off.png", nullptr);
    mixer_solo_OverIcon= new LOSPixmap(":/images/icons/mixer_solo_new_over.png", nullptr);
    mixer_soloIconSet3         = new QIcon();
    mixer_soloIconSet3->addPixmap(*mixer_solo_OnIcon, QIcon::Normal, QIcon::On);
    mixer_soloIconSet3->addPixmap(*mixer_solo_OffIcon, QIcon::Normal, QIcon::Off);
    mixer_soloIconSet3->addPixmap(*mixer_solo_OverIcon, QIcon::Active);

    mixer_input_OffIcon= new LOSPixmap(":/images/icons/mixer_input_new_off.png", nullptr);
    mixer_input_OverIcon= new LOSPixmap(":/images/icons/mixer_input_new_over.png", nullptr);
    mixer_inputIconSet3         = new QIcon();
    mixer_inputIconSet3->addPixmap(*mixer_input_OffIcon, QIcon::Normal, QIcon::Off);
    mixer_inputIconSet3->addPixmap(*mixer_input_OverIcon, QIcon::Active);

    mixer_output_OffIcon= new LOSPixmap(":/images/icons/mixer_output_new_off.png", nullptr);
    mixer_output_OverIcon= new LOSPixmap(":/images/icons/mixer_output_new_over.png", nullptr);
    mixer_outputIconSet3         = new QIcon();
    mixer_outputIconSet3->addPixmap(*mixer_output_OffIcon, QIcon::Normal, QIcon::Off);
    mixer_outputIconSet3->addPixmap(*mixer_output_OverIcon, QIcon::Active);

    mixer_power_OnIcon= new LOSPixmap(":/images/icons/mixer_power_new_on.png", nullptr);
    mixer_power_OffIcon= new LOSPixmap(":/images/icons/mixer_power_new_off.png", nullptr);
    mixer_power_OverIcon= new LOSPixmap(":/images/icons/mixer_power_new_over.png", nullptr);
    mixer_powerIconSet3         = new QIcon();
    mixer_powerIconSet3->addPixmap(*mixer_power_OnIcon, QIcon::Normal, QIcon::On);
    mixer_powerIconSet3->addPixmap(*mixer_power_OffIcon, QIcon::Normal, QIcon::Off);
    mixer_powerIconSet3->addPixmap(*mixer_power_OverIcon, QIcon::Active);

    mixer_blank_OffIcon= new LOSPixmap(":/images/icons/mixer_blank_new_off.png", nullptr);

    mixer_stereo_OnIcon= new LOSPixmap(":/images/icons/mixer_stereo_new_on.png", nullptr);
    mixer_mono_OnIcon= new LOSPixmap(":/images/icons/mixer_mono_new_on.png", nullptr);
    mixer_stereoIconSet3         = new QIcon();
    mixer_stereoIconSet3->addPixmap(*mixer_stereo_OnIcon, QIcon::Normal, QIcon::On);
    mixer_stereoIconSet3->addPixmap(*mixer_mono_OnIcon, QIcon::Normal, QIcon::Off);

    mixer_OnIcon= new LOSPixmap(":/images/icons/mixer_new_on.png", nullptr);
    mixer_OffIcon= new LOSPixmap(":/images/icons/mixer_new_off.png", nullptr);
    mixer_OverIcon= new LOSPixmap(":/images/icons/mixer_new_over.png", nullptr);
    mixerIconSet3         = new QIcon();
    mixerIconSet3->addPixmap(*mixer_OnIcon, QIcon::Normal, QIcon::On);
    mixerIconSet3->addPixmap(*mixer_OffIcon, QIcon::Normal, QIcon::Off);
    mixerIconSet3->addPixmap(*mixer_OverIcon, QIcon::Active);

    pcloader_OnIcon= new LOSPixmap(":/images/icons/pcloader_new_on.png", nullptr);
    pcloader_OffIcon= new LOSPixmap(":/images/icons/pcloader_new_off.png", nullptr);
    pcloader_OverIcon= new LOSPixmap(":/images/icons/pcloader_new_over.png", nullptr);
    pcloaderIconSet3         = new QIcon();
    pcloaderIconSet3->addPixmap(*pcloader_OnIcon, QIcon::Normal, QIcon::On);
    pcloaderIconSet3->addPixmap(*pcloader_OffIcon, QIcon::Normal, QIcon::Off);
    pcloaderIconSet3->addPixmap(*pcloader_OverIcon, QIcon::Active);

    enabled_OnIcon= new LOSPixmap(":/images/icons/enabled_new_on.png", nullptr);
    enabled_OffIcon= new LOSPixmap(":/images/icons/enabled_new_off.png", nullptr);
    enabled_OverIcon= new LOSPixmap(":/images/icons/enabled_new_over.png", nullptr);
    enabledIconSet3         = new QIcon();
    enabledIconSet3->addPixmap(*enabled_OnIcon, QIcon::Normal, QIcon::On);
    enabledIconSet3->addPixmap(*enabled_OffIcon, QIcon::Normal, QIcon::Off);
    enabledIconSet3->addPixmap(*enabled_OverIcon, QIcon::Active);

    up_arrow_OnIcon= new LOSPixmap(":/images/up_arrow_new_on.png", nullptr);
    up_arrow_OffIcon= new LOSPixmap(":/images/up_arrow_new_off.png", nullptr);
    up_arrow_OverIcon= new LOSPixmap(":/images/up_arrow_new_over.png", nullptr);
    up_arrowIconSet3         = new QIcon();
    up_arrowIconSet3->addPixmap(*up_arrow_OnIcon, QIcon::Normal, QIcon::On);
    up_arrowIconSet3->addPixmap(*up_arrow_OffIcon, QIcon::Normal, QIcon::Off);
    up_arrowIconSet3->addPixmap(*up_arrow_OverIcon, QIcon::Active);

    down_arrow_OnIcon= new LOSPixmap(":/images/down_arrow_new_on.png", nullptr);
    down_arrow_OffIcon= new LOSPixmap(":/images/down_arrow_new_off.png", nullptr);
    down_arrow_OverIcon= new LOSPixmap(":/images/down_arrow_new_over.png", nullptr);
    down_arrowIconSet3         = new QIcon();
    down_arrowIconSet3->addPixmap(*down_arrow_OnIcon, QIcon::Normal, QIcon::On);
    down_arrowIconSet3->addPixmap(*down_arrow_OffIcon, QIcon::Normal, QIcon::Off);
    down_arrowIconSet3->addPixmap(*down_arrow_OverIcon, QIcon::Active);

    collapseIconSet3         = new QIcon();
    collapseIconSet3->addPixmap(*up_arrow_OnIcon, QIcon::Normal, QIcon::On);
    collapseIconSet3->addPixmap(*down_arrow_OffIcon, QIcon::Normal, QIcon::Off);
    collapseIconSet3->addPixmap(*up_arrow_OverIcon, QIcon::Active, QIcon::On);
    collapseIconSet3->addPixmap(*down_arrow_OverIcon, QIcon::Active, QIcon::Off);

    plus_OnIcon= new LOSPixmap(":/images/plus_new_on.png", nullptr);
    plus_OffIcon= new LOSPixmap(":/images/plus_new_off.png", nullptr);
    plus_OverIcon= new LOSPixmap(":/images/plus_new_over.png", nullptr);
    plusIconSet3         = new QIcon();
    plusIconSet3->addPixmap(*plus_OnIcon, QIcon::Normal, QIcon::On);
    plusIconSet3->addPixmap(*plus_OffIcon, QIcon::Normal, QIcon::Off);
    plusIconSet3->addPixmap(*plus_OverIcon, QIcon::Active);

    browse_OnIcon= new LOSPixmap(":/images/browse_new_on.png", nullptr);
    browse_OffIcon= new LOSPixmap(":/images/browse_new_off.png", nullptr);
    browse_OverIcon= new LOSPixmap(":/images/browse_new_over.png", nullptr);
    browseIconSet3         = new QIcon();
    browseIconSet3->addPixmap(*browse_OnIcon, QIcon::Normal, QIcon::On);
    browseIconSet3->addPixmap(*browse_OffIcon, QIcon::Normal, QIcon::Off);
    browseIconSet3->addPixmap(*browse_OverIcon, QIcon::Active);

    garbage_OnIcon= new LOSPixmap(":/images/garbage_new_on.png", nullptr);
    garbage_OffIcon= new LOSPixmap(":/images/garbage_new_off.png", nullptr);
    garbage_OverIcon= new LOSPixmap(":/images/garbage_new_over.png", nullptr);
    garbageIconSet3         = new QIcon();
    garbageIconSet3->addPixmap(*garbage_OnIcon, QIcon::Normal, QIcon::On);
    garbageIconSet3->addPixmap(*garbage_OffIcon, QIcon::Normal, QIcon::Off);
    garbageIconSet3->addPixmap(*garbage_OverIcon, QIcon::Active);

    duplicate_OnIcon= new LOSPixmap(":/images/duplicate_new_on.png", nullptr);
    duplicate_OffIcon= new LOSPixmap(":/images/duplicate_new_off.png", nullptr);
    duplicate_OverIcon= new LOSPixmap(":/images/duplicate_new_over.png", nullptr);
    duplicateIconSet3         = new QIcon();
    duplicateIconSet3->addPixmap(*duplicate_OnIcon, QIcon::Normal, QIcon::On);
    duplicateIconSet3->addPixmap(*duplicate_OffIcon, QIcon::Normal, QIcon::Off);
    duplicateIconSet3->addPixmap(*duplicate_OverIcon, QIcon::Active);

    comment_OnIcon= new LOSPixmap(":/images/comment_new_on.png", nullptr);
    comment_OffIcon= new LOSPixmap(":/images/comment_new_off.png", nullptr);
    comment_OverIcon= new LOSPixmap(":/images/comment_new_over.png", nullptr);
    commentIconSet3         = new QIcon();
    commentIconSet3->addPixmap(*comment_OnIcon, QIcon::Normal, QIcon::On);
    commentIconSet3->addPixmap(*comment_OffIcon, QIcon::Normal, QIcon::Off);
    commentIconSet3->addPixmap(*comment_OverIcon, QIcon::Active);

    dragMidiIcon= new LOSPixmap(":/images/drag_midi.png", nullptr);
    dragAudioIcon= new LOSPixmap(":/images/drag_audio.png", nullptr);
    dragInputIcon= new LOSPixmap(":/images/drag_input.png", nullptr);
    dragOutputIcon= new LOSPixmap(":/images/drag_out.png", nullptr);
    dragAuxIcon= new LOSPixmap(":/images/drag_aux.png", nullptr);
    dragBussIcon= new LOSPixmap(":/images/drag_buss.png", nullptr);

    transpose_OnIcon= new LOSPixmap(":/images/transpose_new_on.png", nullptr);
    transpose_OffIcon= new LOSPixmap(":/images/transpose_new_off.png", nullptr);
    transpose_OverIcon= new LOSPixmap(":/images/transpose_new_over.png", nullptr);
    transposeIconSet3         = new QIcon();
    transposeIconSet3->addPixmap(*transpose_OnIcon, QIcon::Normal, QIcon::On);
    transposeIconSet3->addPixmap(*transpose_OffIcon, QIcon::Normal, QIcon::Off);
    transposeIconSet3->addPixmap(*transpose_OverIcon, QIcon::Active);

    connect_OnIcon= new LOSPixmap(":/images/connect_new_on.png", nullptr);
    connect_OffIcon= new LOSPixmap(":/images/connect_new_off.png", nullptr);
    connect_OverIcon= new LOSPixmap(":/images/connect_new_over.png", nullptr);
    connectIconSet3         = new QIcon();
    connectIconSet3->addPixmap(*connect_OnIcon, QIcon::Normal, QIcon::On);
    connectIconSet3->addPixmap(*connect_OffIcon, QIcon::Normal, QIcon::Off);
    connectIconSet3->addPixmap(*connect_OverIcon, QIcon::Active);

    input_OnIcon= new LOSPixmap(":/images/input_new_on.png", nullptr);
    input_OffIcon= new LOSPixmap(":/images/input_new_off.png", nullptr);
    input_OverIcon= new LOSPixmap(":/images/input_new_over.png", nullptr);
    inputIconSet3         = new QIcon();
    inputIconSet3->addPixmap(*input_OnIcon, QIcon::Normal, QIcon::On);
    inputIconSet3->addPixmap(*input_OffIcon, QIcon::Normal, QIcon::Off);
    inputIconSet3->addPixmap(*input_OverIcon, QIcon::Active);

    load_OnIcon= new LOSPixmap(":/images/load_new_on.png", nullptr);
    load_OffIcon= new LOSPixmap(":/images/load_new_off.png", nullptr);
    load_OverIcon= new LOSPixmap(":/images/load_new_over.png", nullptr);
    loadIconSet3         = new QIcon();
    loadIconSet3->addPixmap(*load_OnIcon, QIcon::Normal, QIcon::On);
    loadIconSet3->addPixmap(*load_OffIcon, QIcon::Normal, QIcon::Off);
    loadIconSet3->addPixmap(*load_OverIcon, QIcon::Active);

    pluginGUI_OnIcon= new LOSPixmap(":/images/pluginGUI_new_on.png", nullptr);
    pluginGUI_OffIcon= new LOSPixmap(":/images/pluginGUI_new_off.png", nullptr);
    pluginGUI_OverIcon= new LOSPixmap(":/images/pluginGUI_new_over.png", nullptr);
    pluginGUIIconSet3         = new QIcon();
    pluginGUIIconSet3->addPixmap(*pluginGUI_OnIcon, QIcon::Normal, QIcon::On);
    pluginGUIIconSet3->addPixmap(*pluginGUI_OffIcon, QIcon::Normal, QIcon::Off);
    pluginGUIIconSet3->addPixmap(*pluginGUI_OverIcon, QIcon::Active);

    route_edit_OnIcon= new LOSPixmap(":/images/route_edit_new_on.png", nullptr);
    route_edit_OffIcon= new LOSPixmap(":/images/route_edit_new_off.png", nullptr);
    route_edit_OverIcon= new LOSPixmap(":/images/route_edit_new_over.png", nullptr);
    route_editIconSet3         = new QIcon();
    route_editIconSet3->addPixmap(*route_edit_OnIcon, QIcon::Normal, QIcon::On);
    route_editIconSet3->addPixmap(*route_edit_OffIcon, QIcon::Normal, QIcon::Off);
    route_editIconSet3->addPixmap(*route_edit_OverIcon, QIcon::Active);

    next_OnIcon= new LOSPixmap(":/images/next_new_on.png", nullptr);
    next_OffIcon= new LOSPixmap(":/images/next_new_off.png", nullptr);
    next_OverIcon= new LOSPixmap(":/images/next_new_over.png", nullptr);
    nextIconSet3         = new QIcon();
    nextIconSet3->addPixmap(*next_OnIcon, QIcon::Normal, QIcon::On);
    nextIconSet3->addPixmap(*next_OffIcon, QIcon::Normal, QIcon::Off);
    nextIconSet3->addPixmap(*next_OverIcon, QIcon::Active);

    previous_OnIcon= new LOSPixmap(":/images/previous_new_on.png", nullptr);
    previous_OffIcon= new LOSPixmap(":/images/previous_new_off.png", nullptr);
    previous_OverIcon= new LOSPixmap(":/images/previous_new_over.png", nullptr);
    previousIconSet3         = new QIcon();
    previousIconSet3->addPixmap(*previous_OnIcon, QIcon::Normal, QIcon::On);
    previousIconSet3->addPixmap(*previous_OffIcon, QIcon::Normal, QIcon::Off);
    previousIconSet3->addPixmap(*previous_OverIcon, QIcon::Active);

    redLedIcon           = new LOSPixmap(redled_xpm, nullptr);
    darkRedLedIcon       = new LOSPixmap(darkredled_xpm, nullptr);
    greendotIcon         = new LOSPixmap(greendot_xpm, nullptr);
    bluedotIcon          = new LOSPixmap(bluedot_xpm, nullptr);
    graydotIcon          = new LOSPixmap(graydot_xpm, nullptr);
    offIcon              = new LOSPixmap(off_xpm, nullptr);
    blacksquareIcon      = new LOSPixmap(blacksquare_xpm, nullptr);
    blacksqcheckIcon     = new LOSPixmap(blacksqcheck_xpm, nullptr);

    mastertrackSIcon     = new LOSPixmap(mastertrackS_xpm, nullptr);
    localoffSIcon        = new LOSPixmap(localoffS_xpm, nullptr);
    miditransformSIcon   = new LOSPixmap(miditransformS_xpm, nullptr);
    midi_plugSIcon       = new LOSPixmap(midi_plugS_xpm, nullptr);
    miditransposeSIcon   = new LOSPixmap(":/images/icons/arrow_switch.png", nullptr);
    commentIcon          = new LOSPixmap(":/images/icons/comment.png", nullptr);
    midiThruOnIcon       = new LOSPixmap(midi_thru_on3_xpm, nullptr);
    midiThruOffIcon      = new LOSPixmap(midi_thru_off3_xpm, nullptr);

    mustangSIcon         = new LOSPixmap(mustangS_xpm, nullptr);
    resetSIcon           = new LOSPixmap(resetS_xpm, nullptr);
    track_addIcon        = new LOSPixmap(track_add_xpm, nullptr);
    track_deleteIcon     = new LOSPixmap(track_delete_xpm, nullptr);
    listSIcon            = new LOSPixmap(listS_xpm, nullptr);
    inputpluginSIcon     = new LOSPixmap(inputpluginS_xpm, nullptr);
    cliplistSIcon        = new LOSPixmap(cliplistS_xpm, nullptr);
    mixerAudioSIcon      = new LOSPixmap(":/images/icons/mixeraudioS.png", nullptr);
    initSIcon            = new LOSPixmap(initS_xpm, nullptr);
    songInfoIcon         = new LOSPixmap(":/images/icons/initSong.png", nullptr);

    addMidiIcon     = new LOSPixmap(":/images/icons/views_midi_menu.png", nullptr);
    addBussIcon     = new LOSPixmap(":/images/icons/views_busses_menu.png", nullptr);
    addInputIcon    = new LOSPixmap(":/images/icons/views_inputs_menu.png", nullptr);
    addOutputIcon   = new LOSPixmap(":/images/icons/views_outputs_menu.png", nullptr);
    addAuxIcon      = new LOSPixmap(":/images/icons/views_auxs_menu.png", nullptr);
    addAudioIcon    = new LOSPixmap(":/images/icons/views_audio_menu.png", nullptr);
    addSynthIcon    = new LOSPixmap(":/images/icons/views_synth_menu.png", nullptr);

    addtrack_addmiditrackIcon     = new LOSPixmap(addtrack_addmiditrack_xpm, nullptr);
    addtrack_audiogroupIcon       = new LOSPixmap(addtrack_audiogroup_xpm, nullptr);
    addtrack_audioinputIcon       = new LOSPixmap(addtrack_audioinput_xpm, nullptr);
    addtrack_audiooutputIcon      = new LOSPixmap(addtrack_audiooutput_xpm, nullptr);
    addtrack_auxsendIcon          = new LOSPixmap(addtrack_auxsend_xpm, nullptr);
    addtrack_drumtrackIcon        = new LOSPixmap(addtrack_drumtrack_xpm, nullptr);
    addtrack_wavetrackIcon        = new LOSPixmap(addtrack_wavetrack_xpm, nullptr);
    edit_drummsIcon               = new LOSPixmap(edit_drumms_xpm, nullptr);
    edit_listIcon                 = new LOSPixmap(edit_list_xpm, nullptr);
    remove_ctrlIcon               = new LOSPixmap(remove_ctrl_xpm, nullptr);
    edit_waveIcon                 = new LOSPixmap(edit_wave_xpm, nullptr);
    edit_mastertrackIcon          = new LOSPixmap(edit_mastertrack_xpm, nullptr);
    edit_pianorollIcon            = new LOSPixmap(edit_pianoroll_xpm, nullptr);
    edit_scoreIcon                = new LOSPixmap(edit_score_xpm, nullptr);
    edit_track_addIcon            = new LOSPixmap(edit_track_add_xpm, nullptr);
    edit_track_delIcon            = new LOSPixmap(edit_track_del_xpm, nullptr);
    mastertrack_graphicIcon       = new LOSPixmap(mastertrack_graphic_xpm, nullptr);
    mastertrack_listIcon          = new LOSPixmap(mastertrack_list_xpm, nullptr);
    midi_transformIcon            = new LOSPixmap(midi_transform_xpm, nullptr);
    midi_transposeIcon            = new LOSPixmap(midi_transpose_xpm, nullptr);
    selectIcon                    = new LOSPixmap(select_xpm, nullptr);
    selectMultiIcon               = new LOSPixmap(":/images/icons/select_multi.png", nullptr);
    select_allIcon                = new LOSPixmap(select_all_xpm, nullptr);
    select_all_parts_on_trackIcon = new LOSPixmap(select_all_parts_on_track_xpm, nullptr);
    select_deselect_allIcon       = new LOSPixmap(":/images/icons/deselectall.png", nullptr);
    select_inside_loopIcon        = new LOSPixmap(select_inside_loop_xpm, nullptr);
    select_invert_selectionIcon   = new LOSPixmap(":/images/icons/select_invert_selection.png", nullptr);
    select_outside_loopIcon       = new LOSPixmap(select_outside_loop_xpm, nullptr);
    pianoIconSet                  = new LOSIcon(edit_pianoroll_xpm, nullptr); // ddskrjo

    audio_bounce_to_fileIcon                      = new LOSPixmap(audio_bounce_to_file_xpm, nullptr);
    audio_bounce_to_trackIcon                     = new LOSPixmap(audio_bounce_to_track_xpm, nullptr);
    audio_restartaudioIcon                        = new LOSPixmap(audio_restartaudio_xpm, nullptr);
    automation_clear_dataIcon                     = new LOSPixmap(automation_clear_data_xpm, nullptr);
    automation_take_snapshotIcon                  = new LOSPixmap(automation_take_snapshot_xpm, nullptr);
    edit_midiIcon                                 = new LOSPixmap(edit_midi_xpm, nullptr);
    midi_edit_instrumentIcon                      = new LOSPixmap(midi_edit_instrument_xpm, nullptr);
    midi_init_instrIcon                           = new LOSPixmap(midi_init_instr_xpm, nullptr);
    midi_inputpluginsIcon                         = new LOSPixmap(midi_inputplugins_xpm, nullptr);
    midi_inputplugins_midi_input_filterIcon       = new LOSPixmap(midi_inputplugins_midi_input_filter_xpm, nullptr);
    midi_inputplugins_midi_input_transformIcon    = new LOSPixmap(midi_inputplugins_midi_input_transform_xpm, nullptr);
    midi_inputplugins_random_rhythm_generatorIcon = new LOSPixmap(midi_inputplugins_random_rhythm_generator_xpm, nullptr);
    midi_inputplugins_remote_controlIcon          = new LOSPixmap(midi_inputplugins_remote_control_xpm, nullptr);
    midi_inputplugins_transposeIcon               = new LOSPixmap(midi_inputplugins_transpose_xpm, nullptr);
    midi_local_offIcon                            = new LOSPixmap(midi_local_off_xpm, nullptr);
    midi_reset_instrIcon                          = new LOSPixmap(midi_reset_instr_xpm, nullptr);
    settings_appearance_settingsIcon              = new LOSPixmap(settings_appearance_settings_xpm, nullptr);
    settings_configureshortcutsIcon               = new LOSPixmap(settings_configureshortcuts_xpm, nullptr);
    settings_follow_songIcon                      = new LOSPixmap(settings_follow_song_xpm, nullptr);
    settings_globalsettingsIcon                   = new LOSPixmap(settings_globalsettings_xpm, nullptr);
    settings_midifileexportIcon                   = new LOSPixmap(settings_midifileexport_xpm, nullptr);
    settings_midiport_softsynthsIcon              = new LOSPixmap(settings_midiport_softsynths_xpm, nullptr);
    settings_midisyncIcon                         = new LOSPixmap(settings_midisync_xpm, nullptr);
    view_bigtime_windowIcon                       = new LOSPixmap(view_bigtime_window_xpm, nullptr);
    view_cliplistIcon                             = new LOSPixmap(view_cliplist_xpm, nullptr);
    view_markerIcon                               = new LOSPixmap(view_marker_xpm, nullptr);
    view_transport_windowIcon                     = new LOSPixmap(view_transport_window_xpm, nullptr);

    mixerIcon                                     = new LOSPixmap(":/images/icons/showmixer.png", nullptr);
    expandIcon                                    = new LOSPixmap(":/images/icons/mixer-expand.png", nullptr);
    monoIcon                                      = new LOSPixmap(":/images/icons/mixer-mono.png", nullptr);
    stereoIcon                                    = new LOSPixmap(":/images/icons/mixer-stereo.png", nullptr);
    automationIcon                                = new LOSPixmap(":/images/icons/automation.png", nullptr);
    portIcon                                		= new LOSPixmap(":/images/icons/ports.png", nullptr);
    automationDisabledIcon                        = new LOSPixmap(":/images/icons/automation_disabled.png", nullptr);
    portDisabledIcon                              = new LOSPixmap(":/images/icons/ports_disabled.png", nullptr);
    addTVIcon                              		= new LOSPixmap(":/images/icons/add_tv.png", nullptr);
    reminder1_OnIcon                              = new LOSPixmap(":/images/icons/reminder1_new_on.png", nullptr);
    reminder1_OffIcon                        		= new LOSPixmap(":/images/icons/reminder1_new_off.png", nullptr);
    reminder1_OverIcon                       		= new LOSPixmap(":/images/icons/reminder1_new_over.png", nullptr);
    reminder2_OnIcon                              = new LOSPixmap(":/images/icons/reminder2_new_on.png", nullptr);
    reminder2_OffIcon                        		= new LOSPixmap(":/images/icons/reminder2_new_off.png", nullptr);
    reminder2_OverIcon                       		= new LOSPixmap(":/images/icons/reminder2_new_over.png", nullptr);
    reminder3_OnIcon                              = new LOSPixmap(":/images/icons/reminder3_new_on.png", nullptr);
    reminder3_OffIcon                        		= new LOSPixmap(":/images/icons/reminder3_new_off.png", nullptr);
    reminder3_OverIcon                       		= new LOSPixmap(":/images/icons/reminder3_new_over.png", nullptr);

    record_track_OnIcon                           = new LOSPixmap(":/images/icons/record_track_new_on.png", nullptr);
    record_track_OffIcon                          = new LOSPixmap(":/images/icons/record_track_new_off.png", nullptr);
    record_track_OverIcon                       	= new LOSPixmap(":/images/icons/record_track_new_over.png", nullptr);

    mute_track_OnIcon                          	= new LOSPixmap(":/images/icons/mute_track_new_on.png", nullptr);
    mute_track_OffIcon                         	= new LOSPixmap(":/images/icons/mute_track_new_off.png", nullptr);
    mute_track_OverIcon                        	= new LOSPixmap(":/images/icons/mute_track_new_over.png", nullptr);

    solo_track_OnIcon                          	= new LOSPixmap(":/images/icons/solo_track_new_on.png", nullptr);
    solo_track_OffIcon                         	= new LOSPixmap(":/images/icons/solo_track_new_off.png", nullptr);
    solo_track_OverIcon                        	= new LOSPixmap(":/images/icons/solo_track_new_over.png", nullptr);

    input_indicator_OnIcon                        = new LOSPixmap(":/images/icons/input_indicator_new_on.png", nullptr);
    input_indicator_OffIcon                       = new LOSPixmap(":/images/icons/input_indicator_new_off.png", nullptr);
    input_indicator_OverIcon                      = new LOSPixmap(":/images/icons/input_indicator_new_over.png", nullptr);

    automation_track_OffIcon                      = new LOSPixmap(":/images/icons/automation_track_new_off.png", nullptr);
    automation_track_OverIcon                     = new LOSPixmap(":/images/icons/automation_track_new_over.png", nullptr);

    instrument_track_OffIcon                      = new LOSPixmap(":/images/icons/instrument_track_new_off.png", nullptr);
    instrument_track_OverIcon                     = new LOSPixmap(":/images/icons/instrument_track_new_over.png", nullptr);
    instrument_track_ActiveIcon                   = new LOSPixmap(":/images/icons/input_note.png", nullptr);

    instrument_OnIcon                             = new LOSPixmap(":/images/instrument_new_on.png", nullptr);
    instrument_OffIcon                            = new LOSPixmap(":/images/instrument_new_off.png", nullptr);
    instrument_OverIcon                           = new LOSPixmap(":/images/instrument_new_over.png", nullptr);

    reminder1IconSet3 = new QIcon();
    reminder1IconSet3->addPixmap(*reminder1_OnIcon, QIcon::Normal, QIcon::On);
    reminder1IconSet3->addPixmap(*reminder1_OffIcon, QIcon::Normal, QIcon::Off);
    reminder1IconSet3->addPixmap(*reminder1_OverIcon, QIcon::Active);

    reminder2IconSet3 = new QIcon();
    reminder2IconSet3->addPixmap(*reminder2_OnIcon, QIcon::Normal, QIcon::On);
    reminder2IconSet3->addPixmap(*reminder2_OffIcon, QIcon::Normal, QIcon::Off);
    reminder2IconSet3->addPixmap(*reminder2_OverIcon, QIcon::Active);

    reminder3IconSet3 = new QIcon();
    reminder3IconSet3->addPixmap(*reminder3_OnIcon, QIcon::Normal, QIcon::On);
    reminder3IconSet3->addPixmap(*reminder3_OffIcon, QIcon::Normal, QIcon::Off);
    reminder3IconSet3->addPixmap(*reminder3_OverIcon, QIcon::Active);

    record_trackIconSet3 = new QIcon();
    record_trackIconSet3->addPixmap(*record_track_OnIcon, QIcon::Normal, QIcon::On);
    record_trackIconSet3->addPixmap(*record_track_OffIcon, QIcon::Normal, QIcon::Off);
    record_trackIconSet3->addPixmap(*record_track_OverIcon, QIcon::Active);

    mute_trackIconSet3 = new QIcon();
    mute_trackIconSet3->addPixmap(*mute_track_OnIcon, QIcon::Normal, QIcon::On);
    mute_trackIconSet3->addPixmap(*mute_track_OffIcon, QIcon::Normal, QIcon::Off);
    mute_trackIconSet3->addPixmap(*mute_track_OverIcon, QIcon::Active);

    solo_trackIconSet3 = new QIcon();
    solo_trackIconSet3->addPixmap(*solo_track_OnIcon, QIcon::Normal, QIcon::On);
    solo_trackIconSet3->addPixmap(*solo_track_OffIcon, QIcon::Normal, QIcon::Off);
    solo_trackIconSet3->addPixmap(*solo_track_OverIcon, QIcon::Active);

    input_indicatorIconSet3 = new QIcon();
    input_indicatorIconSet3->addPixmap(*input_indicator_OnIcon, QIcon::Normal, QIcon::On);
    input_indicatorIconSet3->addPixmap(*input_indicator_OffIcon, QIcon::Normal, QIcon::Off);
    input_indicatorIconSet3->addPixmap(*input_indicator_OverIcon, QIcon::Active);

    automation_trackIconSet3 = new QIcon();
    automation_trackIconSet3->addPixmap(*automation_track_OffIcon, QIcon::Normal, QIcon::Off);
    automation_trackIconSet3->addPixmap(*automation_track_OverIcon, QIcon::Active);

    instrument_trackIconSet3 = new QIcon();
    instrument_trackIconSet3->addPixmap(*instrument_track_OffIcon, QIcon::Normal, QIcon::Off);
    instrument_trackIconSet3->addPixmap(*instrument_track_OverIcon, QIcon::Active);

    instrumentIconSet3 = new QIcon();
    instrumentIconSet3->addPixmap(*instrument_OnIcon, QIcon::Normal, QIcon::On);
    instrumentIconSet3->addPixmap(*instrument_OffIcon, QIcon::Normal, QIcon::Off);
    instrumentIconSet3->addPixmap(*instrument_OverIcon, QIcon::Active);

    losIcon         = new LOSPixmap(oom_icon_xpm, nullptr);
    aboutLOSImage   = new LOSPixmap(":/images/oom-about.png", nullptr);
    losLeftSideLogo = new LOSPixmap(":/images/oom_leftside_logo.png", nullptr);
    globalIcon      = new LOSIcon(global_xpm, nullptr);//"folder");
    userIcon        = new LOSIcon(user_xpm, nullptr);//"user-home");
    projectIcon     = new LOSIcon(project_xpm, nullptr);//"folder-sound");

    sineIcon        = new LOSPixmap(sine_xpm, nullptr);
    sawIcon         = new LOSPixmap(saw_xpm, nullptr);
}

