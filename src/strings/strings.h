
#ifndef STRINGS_STRINGS_H
#define STRINGS_STRINGS_H

enum Languages {
    LangEnglish,
    LangScots,
    LangScotsGalic,
    LangItalian,
    LangCount,
};

enum StringIndex {
    STR_EMPTY,
    STR_SINGLE_PLAYER,
    STR_CO_OP,
    STR_BACK,
    STR_PLAYERS,

    STR_CANCEL,
    STR_ERASE_CONFIRM,
    STR_ARE_YOU_SURE,

    STR_NEW_GAME,

    STR_CONTINUE,
    STR_LEVEL_SELECT,
    STR_ERASE,

    STR_PAUSED,

    STR_SOUND_HIGH,
    STR_SOUND_MEDIUM,
    STR_SOUND_LOW,
    STR_SOUND_OFF,

    STR_MUSIC_HIGH,
    STR_MUSIC_MEDIUM,
    STR_MUSIC_LOW,
    STR_MUSIC_OFF,

    STR_RESUME,
    STR_RESTART_LEVEL,
    STR_SAVE,
    STR_SAVED,
    STR_SAVE_AND_QUIT,

    STR_MOVE,
    STR_JUMP,
    STR_ATTACK,
    STR_SWITCH,
    STR_CAMERA,
    STR_MAP,
    STR_SKIP,

    STR_MAP_MOVE,
    STR_MAP_VIEW,
    STR_MAP_BACK,

    STR_LANGUAGE,

    STR_LVL_cadet_intro,
    STR_LVL_switch_intro,
    STR_LVL_robot_intro,
    STR_LVL_robot_platform,
    STR_LVL_switch_sizes,
    STR_LVL_lava_platform,
    STR_LVL_connected_colors,
    STR_LVL_switch_tagteam,
    STR_LVL_inverted_doors,
    STR_LVL_platform_intro,
    STR_LVL_spiral,
    STR_LVL_loop_around,
    STR_LVL_radioactive_river,
    STR_LVL_canyon,
    STR_LVL_track_blockage,
    STR_LVL_raised_platform,
    STR_LVL_moving_platforms,
    STR_LVL_switch_mania,
    STR_LVL_raise_robot,
    STR_LVL_final,

    STR_COUNT,
};

extern char gSelectedLanguage;
extern char* gStrings[LangCount][STR_COUNT];

char* getStr(enum StringIndex name);


#endif