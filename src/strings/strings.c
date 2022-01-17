
#include "strings.h"


char* getStr(enum StringIndex name) {
    if (name >= STR_COUNT) {
        return (char*)name;
    } else if (gStrings[gSelectedLanguage][name]) {
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

        [STR_LANGUAGE] = "English",

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

        [STR_LANGUAGE] = "Scots",

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
        [STR_SINGLE_PLAYER] = "Cluichidair Singilte",
        [STR_CO_OP] = "Co-ob",
        [STR_BACK] = "Air Ais",
        [STR_PLAYERS] = "Iomairtichean",

        [STR_CANCEL] = "Sguir Dheth",
        [STR_ERASE_CONFIRM] = "Suath e",
        [STR_ARE_YOU_SURE] = "A bheil thu cinnteach?",

        [STR_NEW_GAME] = "Geama \003ur",

        [STR_CONTINUE] = "Buanachadh",
        [STR_LEVEL_SELECT] = "Tagh mapa",
        [STR_ERASE] = "Cuir dhan sguadal e",

        [STR_PAUSED] = "S\003oradh",

        [STR_SOUND_HIGH] = "Uaim - \004Ard",
        [STR_SOUND_MEDIUM] = "Uaim - Meadhanail",
        [STR_SOUND_LOW] = "Uaim - \004Iosal",
        [STR_SOUND_OFF] = "Uaim - Cuir dheth",

        [STR_MUSIC_HIGH] = "Ce\003ol - \004Ard",
        [STR_MUSIC_MEDIUM] = "Ce\003ol - Meadhanail",
        [STR_MUSIC_LOW] = "Ce\003ol - \004Iosal",
        [STR_MUSIC_OFF] = "Ce\003ol - Cuir dheth ",

        [STR_RESUME] = "Buanachadh",
        [STR_RESTART_LEVEL] = "Ath-toisichadh",
        [STR_SAVE] = "S\003abhail",
        [STR_SAVED] = "Sh\003abhail",
        [STR_SAVE_AND_QUIT] = "S\003abhail agus dùin",

        [STR_MOVE] = "Caraich",
        [STR_JUMP] = "Leum",
        [STR_ATTACK] = "Sl\003araigeadh",
        [STR_SWITCH] = "Suidseadh",
        [STR_CAMERA] = "Camara",
        [STR_MAP] = "Cl\003ar-d\003uthcha",
        [STR_SKIP] = "Sgiobadh",

        [STR_MAP_MOVE] = "Sioftaigeadh",
        [STR_MAP_VIEW] = "Seall",
        [STR_MAP_BACK] = "Air Ais",

        [STR_LANGUAGE] = "G\003aidhlig",

        [STR_LVL_cadet_intro] = "Leum Bhuaithe!",
        [STR_LVL_switch_intro] = "Dorasan agus Suidsean",
        [STR_LVL_robot_intro] = "R\003obot Caraid M\003or",
        [STR_LVL_robot_platform] = "B\003ord Spring an Robot",
        [STR_LVL_switch_sizes] = "Steafag M\003or",
        [STR_LVL_lava_platform] = "R\003eidio-be\003o",
        [STR_LVL_connected_colors] = "Dathan Cheangailte",
        [STR_LVL_switch_tagteam] = "Suidse Sgioba",
        [STR_LVL_inverted_doors] = "Dorasan Ais-thionndaichte",
        [STR_LVL_platform_intro] = "Cl\003aran-gluasadaiche",
        [STR_LVL_spiral] = "Cuachag",
        [STR_LVL_loop_around] = "Rach mun Cuairt",
        [STR_LVL_radioactive_river] = "Crois-abhainn",
        [STR_LVL_canyon] = "Canyon",
        [STR_LVL_track_blockage] = "Traca Suidseadh",
        [STR_LVL_raised_platform] = "Bun-adhbhar \004Ardaichte",
        [STR_LVL_moving_platforms] = "Cl\003aran-gluasadaiche",
        [STR_LVL_switch_mania] = "Ioma-shlighe do Suidsean",
        [STR_LVL_raise_robot] = "Togadair an R\003obot",
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

        [STR_LANGUAGE] = "Italiano",

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

    [LangSpanish] = {
        [STR_SINGLE_PLAYER] = "Un Jugador",
        [STR_CO_OP] = "Cooperativa",
        [STR_BACK] = "Volver",
        [STR_PLAYERS] = "Jugadores",

        [STR_CANCEL] = "Cancelar",
        [STR_ERASE_CONFIRM] = "Borrar",
        [STR_ARE_YOU_SURE] = "\006Seguro?",

        [STR_NEW_GAME] = "Juego Nuevo",

        [STR_CONTINUE] = "Continuar",
        [STR_LEVEL_SELECT] = "Electionar Nivel",
        [STR_ERASE] = "Borrar",

        [STR_PAUSED] = "Pausado",

        [STR_SOUND_HIGH] = "Sonido - Alto",
        [STR_SOUND_MEDIUM] = "Sonido - Mediano",
        [STR_SOUND_LOW] = "Sonido - Bajo",
        [STR_SOUND_OFF] = "Sonido - Apagado",

        [STR_MUSIC_HIGH] = "Musica - Alto",
        [STR_MUSIC_MEDIUM] = "Musica - Mediano",
        [STR_MUSIC_LOW] = "Musica - Bajo",
        [STR_MUSIC_OFF] = "Musica - Apagado",

        [STR_RESUME] = "Volver",
        [STR_RESTART_LEVEL] = "Empezar de Nuevo",
        [STR_SAVE] = "Guardar",
        [STR_SAVED] = "Guardado",
        [STR_SAVE_AND_QUIT] = "Guardar y Salir",

        [STR_MOVE] = "Mover",
        [STR_JUMP] = "Saltar",
        [STR_ATTACK] = "Atacar",
        [STR_SWITCH] = "Cambiar",
        [STR_CAMERA] = "C\003amera",
        [STR_MAP] = "Mapa",
        [STR_SKIP] = "Pasar",

        [STR_MAP_MOVE] = "Mover",
        [STR_MAP_VIEW] = "Vista",
        [STR_MAP_BACK] = "Volver",

        [STR_LANGUAGE] = "Espa\007ol",

        [STR_LVL_cadet_intro] = "\005Salta Ya!",
        [STR_LVL_switch_intro] = "Puertas y Interruptores",
        [STR_LVL_robot_intro] = "Cuate Robot Grande",
        [STR_LVL_robot_platform] = "Plataforma Robot",
        [STR_LVL_switch_sizes] = "Interruptore Grande",
        [STR_LVL_lava_platform] = "Radioactivo",
        [STR_LVL_connected_colors] = "Colores Connectados",
        [STR_LVL_switch_tagteam] = "Equipo de Interruptores",
        [STR_LVL_inverted_doors] = "Puertas Invertidas",
        [STR_LVL_platform_intro] = "Plataformas M\003oviles",
        [STR_LVL_spiral] = "Espiral",
        [STR_LVL_loop_around] = "Alrededor",
        [STR_LVL_radioactive_river] = "Traves\003ia de R\003io",
        [STR_LVL_canyon] = "Ca\007\003on",
        [STR_LVL_track_blockage] = "Carril Interpusado",
        [STR_LVL_raised_platform] = "Plataforma Subida",
        [STR_LVL_moving_platforms] = "Plataformas M\003oviles",
        [STR_LVL_switch_mania] = "Laberinto",
        [STR_LVL_raise_robot] = "Robot Subido",
        [STR_LVL_final] = "Hora de Salir",
    },
};