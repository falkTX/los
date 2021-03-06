//
// C++ Interface: shortcuts
//
// Description:
// Datastructures and declaration of shortcuts used in the application
//
// Author: Mathias Lundgren <lunar_shuttle@users.sourceforge.net>, (C) 2003
//
// Copyright: Mathias Lundgren (lunar_shuttle@users.sourceforge.net) (C) 2003
//
//
#ifndef __SHORTCUTS_H__
#define __SHORTCUTS_H__

#include <list>
#include "xml.h"

//
// Shortcut categories
//
#define PROLL_SHRT       1  // Pianoroll shortcut
//#define DEDIT_SHRT       2  // Drumedit shortcut
#define LEDIT_SHRT       4  // Listedit shortcut
//#define SCORE_SHRT       8  // Score shortcut
#define ARRANG_SHRT     16  // Arranger shortcut
#define TRANSP_SHRT     32  // Transport shortcut
//#define WAVE_SHRT       64  // Waveedit shortcut
#define GLOBAL_SHRT    128  // Global shortcuts
#define LMEDIT_SHRT    256  // List masteredit
#define MEDIT_SHRT     512  // Master editor
#define ALL_SHRT      1023  // All shortcuts
#define INVIS_SHRT    1024  // Shortcuts not shown in the config-dialog. Hard-coded. To avoid conflicts
#define MIXER_SHRT    1025  // Shortcuts directly related to mixer.

#define SHRT_NUM_OF_CATEGORIES   6 //Number of shortcut categories

struct shortcut
{
    int key;
    const char* descr;
    const char* xml; //xml-tag for config-file
    int type;
};

struct shortcut_cg
{
    int id_flag;
    const char* name;
};

typedef struct shortcut ShortCut;

enum
{
    //Transport/Positioning
    SHRT_PLAY_SONG, //Enter
    SHRT_PLAY_TOGGLE, //Space
    SHRT_STOP, //Insert
    SHRT_GOTO_START, // W
    SHRT_GOTO_LEFT, //End-keypad
    SHRT_GOTO_RIGHT, //Cursordown-keypad
    SHRT_ADD_REST, // Plus
    SHRT_POS_INC, // Plus
    SHRT_POS_DEC, // Minus
    SHRT_TOGGLE_LOOP, // Slash
    SHRT_START_REC, // *(keypad)
    SHRT_REC_CLEAR, // *(keypad)
    SHRT_GOTO_SEL_NOTE,
    SHRT_PLAY_REPEAT, //u
    SHRT_TOGGLE_PLAY_REPEAT, //Ctrl+u toggle audition off

    //Main + Drumeditor
    SHRT_NEW, //Ctrl+N
    SHRT_OPEN, //Ctrl+O
    SHRT_SAVE, //Ctrl+S

    //Used throughout the app:
    SHRT_UNDO, //Ctrl+Z
    SHRT_REDO, //Ctrl+Y
    SHRT_COPY, //Ctrl+C
    SHRT_CUT, //Ctrl+X
    SHRT_PASTE, //Ctrl+V
    SHRT_DELETE, //Delete

    //Main:
    SHRT_SAVE_AS, //Default: undefined
    SHRT_OPEN_RECENT, //Ctrl+1
    SHRT_LOAD_TEMPLATE, //Default: undefined
    SHRT_CONFIG_PRINTER, //Ctrl+P
    SHRT_IMPORT_MIDI, //Default: undefined
    SHRT_EXPORT_MIDI, //Default: undefined
    //SHRT_IMPORT_PART, //!< Import midi part to current track & location, Default: undefined
    //SHRT_IMPORT_AUDIO, //Default: undefined
    SHRT_QUIT, //Default: Ctrl+Q

    SHRT_DESEL_PARTS, //Ctrl+B
    SHRT_SELECT_PRTSTRACK, //Default: undefined
    SHRT_OPEN_PIANO, //Ctrl+E
    SHRT_OPEN_SCORE, //Ctrl+R
    SHRT_OPEN_DRUMS, //Ctrl+D
    SHRT_OPEN_LIST, //Ctrl+L
    SHRT_OPEN_GRAPHIC_MASTER, //Ctrl+M
    SHRT_OPEN_LIST_MASTER, //Ctrl+Shift+M
    SHRT_OPEN_MIDI_TRANSFORM, //Ctrl+T
    SHRT_TOGGLE_RACK, //Default: N

    SHRT_GLOBAL_CUT, //Default: undefined
    SHRT_GLOBAL_INSERT, //Default: undefined
    SHRT_GLOBAL_SPLIT, //Default: undefined
    SHRT_COPY_RANGE, //Default: undefined
    SHRT_CUT_EVENTS, //Default: undefined

    SHRT_OPEN_TRANSPORT, //F11
    SHRT_OPEN_BIGTIME, //F12
    SHRT_OPEN_MARKER, // F9
    SHRT_OPEN_CLIPS, //Default: undefined
    //SHRT_OPEN_ROUTES,

    SHRT_FOLLOW_JUMP, //Default: undefined
    SHRT_FOLLOW_NO, //Default: undefined
    SHRT_FOLLOW_CONTINUOUS, //Default: undefined

    SHRT_GLOBAL_CONFIG, //Default: undefined
    SHRT_CONFIG_SHORTCUTS, //Default: undefined
    SHRT_MIDI_FILE_CONFIG, //Default: undefined
    SHRT_APPEARANCE_SETTINGS, //Default: undefined
    SHRT_CONFIG_MIDI_PORTS, //Default: undefined
    SHRT_CONFIG_AUDIO_PORTS, //Default: undefined
    //SHRT_SAVE_GLOBAL_CONFIG, //Default: undefined

    SHRT_MIDI_EDIT_INSTRUMENTS, //Default: undefined
    SHRT_MIDI_INPUT_TRANSFORM, //Default: undefined
    SHRT_MIDI_INPUT_FILTER, //Default: undefined
    SHRT_MIDI_INPUT_TRANSPOSE, //Default: undefined
    SHRT_MIDI_REMOTE_CONTROL, //Default: undefined
    SHRT_MIDI_RESET, //Default: undefined
    SHRT_MIDI_INIT, //Default: undefined
    SHRT_MIDI_LOCAL_OFF, //Default: undefined

    SHRT_AUDIO_RESTART, //Default: undefined

    SHRT_ADD_MIDI_TRACK, //Default: Ctrl+J
    SHRT_RESET_MIDI, //Ctrl+Alt+Z

    SHRT_OPEN_HELP, //F1
    SHRT_START_WHATSTHIS, //Shift-F1

    //Arranger, parts:
    SHRT_EDIT_PART, //Enter
    SHRT_SEL_ABOVE, //Up
    SHRT_SEL_ABOVE_ADD, //move up and add to selection
    SHRT_SEL_BELOW, //Down
    SHRT_SEL_BELOW_ADD, //move down and add to selection

    SHRT_INSERT, //Ctrl+Shift+I   - insert parts instead of pasting
    SHRT_INSERTMEAS, //Ctrl+Shift+M - insert measures

    SHRT_PASTE_CLONE, //CTRL+SHIFT+Key_V
    SHRT_PASTE_TO_TRACK, //CTRL+Key_B
    SHRT_PASTE_CLONE_TO_TRACK, //CTRL+SHIFT+Key_B
    SHRT_INSERT_PART, //Ctrl+Insert

    //Arranger tracks
    SHRT_SEL_TRACK_BELOW,
    SHRT_SEL_TRACK_ABOVE,
    SHRT_SEL_TRACK_ABOVE_ADD,
    SHRT_SEL_TRACK_BELOW_ADD,
    SHRT_RENAME_TRACK, //Ctrl+R
    SHRT_SEL_ALL_TRACK, //(Windows Key)Mod4+A

    //To be in Arranger, pianoroll & drumeditor. p4.0.10 now globally handled, too.
    SHRT_SELECT_ALL, //Ctrl+A
    SHRT_SELECT_NONE, //
    SHRT_SELECT_ALL_NODES, //Ctrl+Shift+A
    SHRT_SELECT_INVERT, //Ctrl+I
    SHRT_SELECT_ILOOP, //Default: Undefined
    SHRT_SELECT_OLOOP, //Default: Undefined
    SHRT_SELECT_PREV_PART, // Ctrl+-
    SHRT_SELECT_NEXT_PART, // Ctrl++
    SHRT_SEL_LEFT, //left
    SHRT_SEL_LEFT_ADD, //move left and add to selection
    SHRT_SEL_RIGHT, //Right
    SHRT_SEL_RIGHT_ADD, //move right and add to selection
    SHRT_INC_PITCH_OCTAVE,
    SHRT_DEC_PITCH_OCTAVE,
    SHRT_INC_PITCH,
    SHRT_DEC_PITCH,
    SHRT_INC_POS,
    SHRT_DEC_POS,
    SHRT_POS_INC_NOSNAP,
    SHRT_POS_DEC_NOSNAP,
    SHRT_TOGGLE_SOUND,
    SHRT_TOGGLE_ENABLE,

    SHRT_LOCATORS_TO_SELECTION, //Alt+P, currently in Arranger & pianoroll
    SHRT_INSERT_AT_LOCATION, //Shift+CrsrRight
    SHRT_INCREASE_LEN,
    SHRT_DECREASE_LEN,

    SHRT_TOOL_1, //Shift+1 Pointer
    SHRT_TOOL_2, //Shift+2 Pen
    SHRT_TOOL_3, //Shift+3 Rubber
    SHRT_TOOL_4, //Shift+4
    SHRT_TOOL_5, //Shift+5
    SHRT_TOOL_6, //Shift+6
    SHRT_TRANSPOSE, //Default: undefined

    //Shortcuts to be in pianoroll & drumeditor
    SHRT_ZOOM_IN, // Ctrl+PgUp
    SHRT_ZOOM_OUT, // Ctrl+PgDown
    SHRT_VZOOM_IN, // Ctrl+Shift+PgUp
    SHRT_VZOOM_OUT, // Ctrl+Shift+PgDown
    SHRT_GOTO_CPOS, // c
    SHRT_SCROLL_LEFT, // h
    SHRT_SCROLL_RIGHT, // l
    SHRT_SCROLL_UP, // Shift+PgUp
    SHRT_SCROLL_DOWN, // Shift+PgDown
    SHRT_FIXED_LEN, //Alt+L, currently only drumeditor
    SHRT_QUANTIZE, //q
    SHRT_OVER_QUANTIZE, //Default: undefined
    SHRT_ON_QUANTIZE, //Default: undefined
    SHRT_ONOFF_QUANTIZE, //Default: undefined
    SHRT_ITERATIVE_QUANTIZE, //Default: undefined
    SHRT_CONFIG_QUANT, //Default: Ctrl+Alt+Q
    SHRT_MODIFY_GATE_TIME, //Default: undefined
    SHRT_MODIFY_VELOCITY,
    SHRT_CRESCENDO,
    SHRT_DELETE_OVERLAPS,

    SHRT_THIN_OUT,
    SHRT_ERASE_EVENT,
    SHRT_NOTE_SHIFT,
    SHRT_MOVE_CLOCK,
    SHRT_COPY_MEASURE,
    SHRT_ERASE_MEASURE,
    SHRT_DELETE_MEASURE,
    SHRT_CREATE_MEASURE,
    SHRT_SET_QUANT_0, //1 - pianoroll
    SHRT_SET_QUANT_1, //1 - pianoroll
    SHRT_SET_QUANT_2, //2 - pianoroll
    SHRT_SET_QUANT_3, //3 - pianoroll
    SHRT_SET_QUANT_4, //4 - pianoroll
    SHRT_SET_QUANT_5, //5 - pianoroll
    SHRT_SET_QUANT_6, //6 - pianoroll
    SHRT_SET_QUANT_7, //7 - pianoroll
    SHRT_TOGGLE_TRIOL, //t
    SHRT_TOGGLE_PUNCT, //.-keypad
    SHRT_TOGGLE_PUNCT2, // ,
    SHRT_TOGGLE_STEPRECORD,
    SHRT_TOGGLE_STEPQWERTY,
    SHRT_OCTAVE_QWERTY_0,  //0 - Set qwerty step input range starts at C0
    SHRT_OCTAVE_QWERTY_1,  //1 - Set qwerty step input range starts at C1
    SHRT_OCTAVE_QWERTY_2,  //2 - Set qwerty step input range starts at C2
    SHRT_OCTAVE_QWERTY_3,  //3 - Set qwerty step input range starts at C3
    SHRT_OCTAVE_QWERTY_4,  //4 - Set qwerty step input range starts at C4
    SHRT_OCTAVE_QWERTY_5,  //5 - Set qwerty step input range starts at C5
    SHRT_OCTAVE_QWERTY_6,  //6 - Set qwerty step input range starts at C6
    SHRT_NOTE_VELOCITY_UP,
    SHRT_NOTE_VELOCITY_DOWN,

    SHRT_EVENT_COLOR, //e

    SHRT_ADD_PROGRAM, //Add program change backslash
    SHRT_COPY_PROGRAM, //Add program change Alt+backslash
    SHRT_SEL_PROGRAM, //Select program change Ctrl+Alt+S
    SHRT_RMOVE_PROGRAM, //Move program change left Ctrl+Alt+period
    SHRT_LMOVE_PROGRAM, //Move program change right Ctrl+Alt+comma
    SHRT_DEL_PROGRAM, //Delete program change under cursor shit+backslash
    SHRT_LMOVE_SELECT, //Move to next program change left  Ctrl+Alt+left
    SHRT_RMOVE_SELECT, //Move to next program change right Ctrl+Alt+right
    SHRT_SEL_INSTRUMENT,
    SHRT_PREVIEW_INSTRUMENT,
    SHRT_PART_TOGGLE_MUTE,

    // Shortcuts for tools
    // global
    SHRT_TOOL_POINTER, //
    SHRT_TOOL_PENCIL,
    SHRT_TOOL_RUBBER,
    SHRT_MIDI_PANIC,
    SHRT_NAVIGATE_TO_CANVAS,

    // pianoroll and drum editor
    SHRT_TOOL_LINEDRAW,

    // Arranger
    SHRT_TOOL_SCISSORS, // j
    SHRT_TOOL_GLUE,
    SHRT_TOOL_MUTE,

    // global state
    SHRT_TRACK_TOGGLE_SOLO,
    SHRT_TRACK_TOGGLE_MUTE,
    SHRT_TRACK_HEIGHT_DEFAULT,
    SHRT_TRACK_HEIGHT_FULL_SCREEN,
    SHRT_TRACK_HEIGHT_SELECTION_FITS_IN_VIEW,
    SHRT_TRACK_HEIGHT_2,
    SHRT_TRACK_HEIGHT_3,
    SHRT_TRACK_HEIGHT_4,
    SHRT_TRACK_HEIGHT_5,
    SHRT_TRACK_HEIGHT_6,
    SHRT_MOVE_TRACK_UP,
    SHRT_MOVE_TRACK_DOWN,

    //Listeditor:
    SHRT_LE_INS_NOTES, //Ctrl+N
    SHRT_LE_INS_SYSEX, //Ctrl+S
    SHRT_LE_INS_CTRL, //Ctrl+T
    SHRT_LE_INS_META, //Default: undefined
    SHRT_LE_INS_CHAN_AFTERTOUCH, //Ctrl+A
    SHRT_LE_INS_POLY_AFTERTOUCH, //Ctrl+P

    //List master editor:
    SHRT_LM_INS_TEMPO, // Ctrl+T
    SHRT_LM_INS_SIG, // Ctrl+R
    SHRT_LM_EDIT_BEAT, // Ctrl+Shift+E
    SHRT_GLOBAL_EDIT_EVENT_VALUE, // Ctrl+E

    // Marker view
    SHRT_NEXT_MARKER,
    SHRT_PREV_MARKER,

    //Last item:
    SHRT_NUM_OF_ELEMENTS // must be last
};

extern ShortCut shortcuts[SHRT_NUM_OF_ELEMENTS]; //size of last entry
extern void initShortCuts();
extern void writeShortCuts(int level, Xml& xml);
extern void readShortCuts(Xml& xml);
extern const shortcut_cg shortcut_category[SHRT_NUM_OF_CATEGORIES];
#endif
