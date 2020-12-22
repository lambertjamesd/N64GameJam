
#include "strings.h"


char* getStr(enum StringIndex name) {
    if (gStrings[gSelectedLanguage][name]) {
        return gStrings[gSelectedLanguage][name];
    } else if (gStrings[LangEnglish][name]) {
        return gStrings[gSelectedLanguage][name];
    } else {
        return "";
    }
}

char gSelectedLanguage;

char* gStrings[LangCount][STR_COUNT] = {
    {
        [STR_SINGLE_PLAYER] = "Single Player",
        [STR_CO_OP] = "Co-op",
        [STR_BACK] = "Back",
        [STR_PLAYERS] = "Players",

        [STR_CANCEL] = "Cancel",
        [STR_ERASE_CONFIRM] = "Erase",
        [STR_ARE_YOU_SURE] = "Are you sure?",

        [STR_NEW_GAME] = "New Game",

        [STR_CONTINUE] = "Continue",
        [STR_LEVEL_SELECT] = "Level Select",
        [STR_ERASE] = "Erase",

        [STR_PAUSED] = "Paused",

        [STR_SOUND_HIGH] = "Sound - High",
        [STR_SOUND_MEDIUM] = "Sound - Medium",
        [STR_SOUND_LOW] = "Sound - Low",
        [STR_SOUND_OFF] = "Sound - Off",

        [STR_MUSIC_HIGH] = "Music - High",
        [STR_MUSIC_MEDIUM] = "Music - Medium",
        [STR_MUSIC_LOW] = "Music - Low",
        [STR_MUSIC_OFF] = "Music - Off",

        [STR_RESUME] = "Resume",
        [STR_RESTART_LEVEL] = "Restart Level",
        [STR_SAVE] = "Save",
        [STR_SAVED] = "Saved",
        [STR_SAVE_AND_QUIT] = "Save and Quit",

        [STR_MOVE] = "Move",
        [STR_JUMP] = "Jump",
        [STR_ATTACK] = "Attack",
        [STR_SWITCH] = "Switch",
        [STR_CAMERA] = "Camera",
        [STR_MAP] = "Map",
        [STR_SKIP] = "Skip",

        [STR_MAP_MOVE] = "Move",
        [STR_MAP_VIEW] = "View",
        [STR_MAP_BACK] = "Back",

        [STR_LVL_cadet_intro] = "Jump to it!",
        [STR_LVL_switch_intro] = "Doors and Switches",
        [STR_LVL_robot_intro] = "Big Robot Buddy",
        [STR_LVL_robot_platform] = "Robot Springboard",
        [STR_LVL_switch_sizes] = "Big Switch",
        [STR_LVL_lava_platform] = "Radioactive",
        [STR_LVL_connected_colors] = "Connected Colors",
        [STR_LVL_switch_tagteam] = "Switch Tag Team",
        [STR_LVL_inverted_doors] = "Inverted Doors",
        [STR_LVL_platform_intro] = "Moving Platforms",
        [STR_LVL_spiral] = "Spiral",
        [STR_LVL_loop_around] = "Loop Around",
        [STR_LVL_radioactive_river] = "River Crossing",
        [STR_LVL_canyon] = "Canyon",
        [STR_LVL_track_blockage] = "Blocked Track",
        [STR_LVL_raised_platform] = "Raised Platform",
        [STR_LVL_moving_platforms] = "Moving Platforms",
        [STR_LVL_switch_mania] = "Switch Maze",
        [STR_LVL_raise_robot] = "Robot Raiser",
        [STR_LVL_final] = "Time to Leave",
    }
};