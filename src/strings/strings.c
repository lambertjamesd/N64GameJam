
#include "strings.h"


char* getStr(enum StringIndex name) {
    if (gStrings[gSelectedLanguage][name]) {
        return gStrings[gSelectedLanguage][name];
    } else if (gStrings[LangEnglish][name]) {
        return gStrings[LangEnglish][name];
    } else {
        return "";
    }
}

char gSelectedLanguage = LangEnglish;

char* gStrings[LangCount][STR_COUNT] = {
    [LangEnglish] = {
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
    },

    [LangScots] = {
        [STR_SINGLE_PLAYER] = "Singleplayer",
        [STR_CO_OP] = "Co-op",
        [STR_BACK] = "Oot",
        [STR_PLAYERS] = "Players",

        [STR_CANCEL] = "Elide",
        [STR_ERASE_CONFIRM] = "Dae awa",
        [STR_ARE_YOU_SURE] = "Ur ye shuir?",

        [STR_NEW_GAME] = "New Gemm",

        [STR_CONTINUE] = "Haud On",
        [STR_LEVEL_SELECT] = "Chyse Level",
        [STR_ERASE] = "Dae Awa",

        [STR_PAUSED] = "Pause",

        [STR_SOUND_HIGH] = "Soond - Heich",
        [STR_SOUND_MEDIUM] = "Soond - Medium",
        [STR_SOUND_LOW] = "Soond - Doon",
        [STR_SOUND_OFF] = "Soond - Aff",

        [STR_MUSIC_HIGH] = "Muisic - Heich",
        [STR_MUSIC_MEDIUM] = "Muisic - Medium",
        [STR_MUSIC_LOW] = "Muisic - Doon",
        [STR_MUSIC_OFF] = "Muisic - Aff",

        [STR_RESUME] = "Haud On",
        [STR_RESTART_LEVEL] = "Stairt Level Again",
        [STR_SAVE] = "Hain",
        [STR_SAVED] = "Haint",
        [STR_SAVE_AND_QUIT] = "Hain an git oot",

        [STR_MOVE] = "Muive",
        [STR_JUMP] = "Jimp",
        [STR_ATTACK] = "Attack",
        [STR_SWITCH] = "Chynge",
        [STR_CAMERA] = "Camera",
        [STR_MAP] = "Chairt",
        [STR_SKIP] = "Skip",

        [STR_MAP_MOVE] = "Muive",
        [STR_MAP_VIEW] = "Keek",
        [STR_MAP_BACK] = "Oot",

        [STR_LVL_cadet_intro] = "Jimp tae it!",
        [STR_LVL_switch_intro] = "Doors an Switches",
        [STR_LVL_robot_intro] = "Muckle Robot Mate",
        [STR_LVL_robot_platform] = "Robot Springboard",
        [STR_LVL_switch_sizes] = "Muckle Sneck",
        [STR_LVL_lava_platform] = "Radioacteeve",
        [STR_LVL_connected_colors] = "Mairchin Colours",
        [STR_LVL_switch_tagteam] = "Sneck Tag Team",
        [STR_LVL_inverted_doors] = "Tapsalteerie Doors",
        [STR_LVL_platform_intro] = "Muivin Platforms",
        [STR_LVL_spiral] = "Spinner",
        [STR_LVL_loop_around] = "Birl Aroond",
        [STR_LVL_radioactive_river] = "Watter Crossin",
        [STR_LVL_canyon] = "Canyon",
        [STR_LVL_track_blockage] = "Track Jam",
        [STR_LVL_raised_platform] = "Pit up Platform",
        [STR_LVL_moving_platforms] = "Muivin Platforms",
        [STR_LVL_switch_mania] = "Sneck Maze",
        [STR_LVL_raise_robot] = "Robot Raiser",
        [STR_LVL_final] = "Time tae Git Oot",
    },

    [LangScotsGalic] = {
        [STR_SINGLE_PLAYER] = "Cluicheadair Singilte",
        [STR_CO_OP] = "Co-ob",
        [STR_BACK] = "Air Ais",
        [STR_PLAYERS] = "Iomairtichean",

        [STR_CANCEL] = "Sguir Dheth",
        [STR_ERASE_CONFIRM] = "Suath e",
        [STR_ARE_YOU_SURE] = "A bheil thu cinnteach?",

        [STR_NEW_GAME] = "Geama \003ur",

        [STR_CONTINUE] = "Buanachadh",
        [STR_LEVEL_SELECT] = "Taghadh mapa",
        [STR_ERASE] = "Suath e",

        [STR_PAUSED] = "S\003oradh",

        [STR_SOUND_HIGH] = "Uaim - \004Ard",
        [STR_SOUND_MEDIUM] = "Uaim - Meadhanail",
        [STR_SOUND_LOW] = "Uaim - \004Iosal",
        [STR_SOUND_OFF] = "Uaim - Balbh",

        [STR_MUSIC_HIGH] = "Ce\003ol - \004Ard",
        [STR_MUSIC_MEDIUM] = "Ce\003ol - Meadhanail",
        [STR_MUSIC_LOW] = "Ce\003ol - \004Iosal",
        [STR_MUSIC_OFF] = "Ce\003ol - Balbh",

        [STR_RESUME] = "Buanachadh",
        [STR_RESTART_LEVEL] = "Ath-toisichadh Mapa",
        [STR_SAVE] = "Caomhnadh",
        [STR_SAVED] = "Chaomh",
        [STR_SAVE_AND_QUIT] = "Ceomhnadh is tog na si\003uil",

        [STR_MOVE] = "Sioftaigeadh",
        [STR_JUMP] = "Leum",
        [STR_ATTACK] = "Sl\003araigeadh",
        [STR_SWITCH] = "Suidseadh",
        [STR_CAMERA] = "Camara",
        [STR_MAP] = "Cl\003ar-d\003uthcha",
        [STR_SKIP] = "Sgiobadh",

        [STR_MAP_MOVE] = "Sioftaigeadh",
        [STR_MAP_VIEW] = "Seall",
        [STR_MAP_BACK] = "Air Ais",

        [STR_LVL_cadet_intro] = "Leum Bhuaithe!",
        [STR_LVL_switch_intro] = "Dorasan agus Steafagan",
        [STR_LVL_robot_intro] = "R\003obot Caraid M\003or",
        // [STR_LVL_robot_platform] = "Robot Springboard",
        [STR_LVL_switch_sizes] = "Steafag M\003or",
        [STR_LVL_lava_platform] = "Rèidio-be\003o",
        [STR_LVL_connected_colors] = "Dathan Cheangailte",
        // [STR_LVL_switch_tagteam] = "Switch Tag Team",
        [STR_LVL_inverted_doors] = "Dorasan Ais-thionndaichte",
        [STR_LVL_platform_intro] = "Cl\003aran-gluasadaiche",
        [STR_LVL_spiral] = "Cuachag",
        [STR_LVL_loop_around] = "Rach mun Cuairt",
        [STR_LVL_radioactive_river] = "Crois-abhainn",
        [STR_LVL_canyon] = "Canyon",
        [STR_LVL_track_blockage] = "Traca Suidseadh",
        [STR_LVL_raised_platform] = "Bun-adhbhar \003ardaichte",
        [STR_LVL_moving_platforms] = "Cl\003aran-gluasadaiche",
        [STR_LVL_switch_mania] = "Ioma-shlighe do Steafagan",
        // [STR_LVL_raise_robot] = "Robot Raiser",
        [STR_LVL_final] = "T\003im gu dol a-mach",
    },

    [LangItalian] = {
        [STR_SINGLE_PLAYER] = "Giocatore Singolo",
        [STR_CO_OP] = "Cooperazione",
        [STR_BACK] = "Torna",
        [STR_PLAYERS] = "Giocatori",

        [STR_CANCEL] = "Anualla",
        [STR_ERASE_CONFIRM] = "Cancella",
        [STR_ARE_YOU_SURE] = "Sei sicuro?",

        [STR_NEW_GAME] = "Nuova Partita",

        [STR_CONTINUE] = "Continua",
        [STR_LEVEL_SELECT] = "Selezione Livello",
        [STR_ERASE] = "Cancella",

        [STR_PAUSED] = "Pausa",

        [STR_SOUND_HIGH] = "Suono - Alto",
        [STR_SOUND_MEDIUM] = "Suono - Medio",
        [STR_SOUND_LOW] = "Suono - Basso",
        [STR_SOUND_OFF] = "Suono - Spento",

        [STR_MUSIC_HIGH] = "Musica - Alto",
        [STR_MUSIC_MEDIUM] = "Musica - Medio",
        [STR_MUSIC_LOW] = "Musica - Basso",
        [STR_MUSIC_OFF] = "Musica - Spento",

        [STR_RESUME] = "Continua",
        [STR_RESTART_LEVEL] = "Ricommincia Livello",
        [STR_SAVE] = "Salva",
        [STR_SAVED] = "Salvato",
        [STR_SAVE_AND_QUIT] = "Salva e Torna",

        [STR_MOVE] = "Muovere",
        [STR_JUMP] = "Saltare",
        [STR_ATTACK] = "Attaccare",
        [STR_SWITCH] = "Cambiare",
        [STR_CAMERA] = "Telecamera",
        [STR_MAP] = "Carta",
        [STR_SKIP] = "Scappare",

        [STR_MAP_MOVE] = "Muovere",
        [STR_MAP_VIEW] = "Guardare",
        [STR_MAP_BACK] = "Torna",

        [STR_LVL_cadet_intro] = "Salta ad esso!",
        [STR_LVL_switch_intro] = "Porte ed Interruttori",
        [STR_LVL_robot_intro] = "Robot Amico Grande",
        [STR_LVL_robot_platform] = "Trampolino Robot",
        [STR_LVL_switch_sizes] = "Interrutore Grande",
        [STR_LVL_lava_platform] = "Radioattivo",
        [STR_LVL_connected_colors] = "Colori Coordinati",
        [STR_LVL_switch_tagteam] = "Interrutore Tag Team",
        [STR_LVL_inverted_doors] = "Porti Invertiti",
        [STR_LVL_platform_intro] = "Piattaforme in Movimento",
        [STR_LVL_spiral] = "Spirale",
        [STR_LVL_loop_around] = "Giro Attorno",
        [STR_LVL_radioactive_river] = "Attraversare il Fiume",
        [STR_LVL_canyon] = "Canyon",
        [STR_LVL_track_blockage] = "Sentiero Bloccato",
        [STR_LVL_raised_platform] = "Piattaforma Aumentato",
        [STR_LVL_moving_platforms] = "Piattaforme in Movimento",
        [STR_LVL_switch_mania] = "Labrino dell'interrutori",
        [STR_LVL_raise_robot] = "Robot Raiser",
        [STR_LVL_final] = "Ora di Uscire",
    },
};