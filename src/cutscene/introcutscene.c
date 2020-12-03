
#include "introcutscene.h"
#include "src/audio/allseq.h"

struct CutsceneFrame gIntroCutsceneFrames[] = {
    {2.0f, _01_insidecockpit, 0.0f, 0.0f},
    {1.5f, _01_insidecockpit_flash, 0.0f, 0.0f},
    {1.5f, _01_insidecockpit, 0.0f, 0.0f},
    {1.5f, _01_insidecockpit_flash, 0.0f, 0.0f},
    {1.5f, _02_fuelgauge, 0.0f, 0.0f},
    {0.5f, _02_fuelgauge_flash, 0.0f, 0.0f},
    {1.0f, _02_fuelgauge_flash, 44.0f, 0.0f},
    {1.0f, _02_fuelgauge, 44.0f, 44.0f},
    {0.5f, _02_fuelgauge, 0.0f, 88.0f},
    {1.5f, _02_fuelgauge_flash, 0.0f, 88.0f},
    {1.5f, _02_fuelgauge, 0.0f, 88.0f},
    {4.0f, _03_mapcloseup, 0.0f, 0.0f},
    {1.5f, _04_seebutton, 0.0f, 0.0f},
    {1.0f, _05_pushbutton, 0.0f, 0.0f},
    {1.0f, _03_mapcloseup, 0.0f, 0.0f},
    {2.0f, _06_mapchanged, 0.0f, 0.0f},
    {1.5f, _00_rocket, 0.0f, 0.0f},
    {1.5f, _08_rocket_turn, 0.0f, 0.0f},
    {8.0f, _09_see_planet, -17.5, 140.0f},
    {4.0f, _09_see_planet, 0.0f, 0.0f},
};

struct CutsceneEvent gIntroCutsceneEvents[] = {
    {
        0.0f,
        CutsceneEventTypeSeq,
        .seq = {
            _darkSunsSegmentRomStart,
            _darkSunsSegmentRomEnd,
            0, 0, 0, 0,
            0x7fff,
        }
    },
    {
        0.0f,
        CutsceneEventTypeSound,
        .sound = {
            IntroSndCockpit,
            0.5f,
            1.0f,
            0.0f,
            10
        }
    },
    {
        2.0f,
        CutsceneEventTypeSound,
        .sound = {
            IntroSndAlarm1,
            0.5f,
            1.0f,
            0.0f,
            10
        }
    },
    {
        5.0f,
        CutsceneEventTypeSound,
        .sound = {
            IntroSndAlarm1,
            0.5f,
            1.0f,
            0.0f,
            10
        }
    },
    {
        6.5f,
        CutsceneEventTypeSound,
        .sound = {
            IntroSndGemNotice,
            0.5f,
            1.0f,
            0.0f,
            10
        }
    },
    {
        8.0f,
        CutsceneEventTypeSound,
        .sound = {
            IntroSndAlarm1,
            0.5f,
            1.0f,
            0.0f,
            10
        }
    },
    {
        11.0f,
        CutsceneEventTypeSound,
        .sound = {
            IntroSndAlarm1,
            0.5f,
            1.0f,
            0.0f,
            10
        }
    },
    {
        11.5f,
        CutsceneEventTypeSound,
        .sound = {
            IntroSndGemNotice2,
            0.5f,
            1.0f,
            0.0f,
            10
        }
    },
    {
        14.0f,
        CutsceneEventTypeSound,
        .sound = {
            IntroSndAlarm1,
            0.5f,
            1.0f,
            0.0f,
            10
        }
    },
    {
        14.5f,
        CutsceneEventTypeSound,
        .sound = {
            IntroSndGemHudGlow,
            0.5f,
            1.0f,
            0.0f,
            10
        }
    },
    {
        17.0f,
        CutsceneEventTypeSound,
        .sound = {
            IntroSndAlarm1,
            0.5f,
            1.0f,
            0.0f,
            10
        }
    },
    {
        19.5f,
        CutsceneEventTypeSeqStop,
    },
    {
        19.5f,
        CutsceneEventTypeSound,
        .sound = {
            IntroSndGoButtonPress,
            0.5f,
            1.0f,
            0.0f,
            10
        }
    },
    {
        20.0f,
        CutsceneEventTypeSound,
        .sound = {
            IntroSndProcessing,
            0.5f,
            1.0f,
            0.0f,
            10
        }
    },
    {
        20.5f,
        CutsceneEventTypeSeq,
        .seq = {
            _cosmicDustSegmentRomStart,
            _cosmicDustSegmentRomEnd,
            9600, 0, 0, 0,
            0x4000,
        }
    },
    {
        23.5f,
        CutsceneEventTypeSoundStop,
        .sound = {IntroSndCockpit}
    },
    {
        24.0f,
        CutsceneEventTypeSound,
        .sound = {
            IntroSndSpaceshipFlyby,
            0.5f,
            1.0f,
            0.0f,
            10
        }
    },
};

struct Cutscene gIntroCutscene = {
    gIntroCutsceneFrames,
    sizeof(gIntroCutsceneFrames)/sizeof(*gIntroCutsceneFrames),
    gIntroCutsceneEvents,
    sizeof(gIntroCutsceneEvents)/sizeof(*gIntroCutsceneEvents),
    SoundBankIntroCutscene,
};