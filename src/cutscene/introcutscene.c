
#include "introcutscene.h"
#include "src/audio/allseq.h"

struct CutsceneFrame gIntroCutsceneFrames[] = {
    {3.0f, 1, 0.0f, 0.0f},
    {3.0f, 2, 0.0f, 0.0f},
    {2.0f, 2, 44.0f, 0.0f},
    {3.0f, 2, 0.0f, 88.0f},
    {4.0f, 3, 0.0f, 0.0f},
    {1.5f, 4, 0.0f, 0.0f},
    {1.0f, 5, 0.0f, 0.0f},
    {1.0f, 3, 0.0f, 0.0f},
    {2.0f, 6, 0.0f, 0.0f},
    {1.5f, 0, 0.0f, 0.0f},
    {1.0f, 7, 0.0f, 0.0f},
    {8.0f, 8, -17.5, 140.0f},
    {4.0f, 8, 0.0f, 0.0f},
};

struct CutsceneEvent gIntroCutsceneEvents[] = {
    {
        0.0f,
        CutsceneEventTypeSeq,
        .seq = {
            _darkSunsSegmentRomStart,
            _darkSunsSegmentRomEnd,
            0, 0, 0, 0
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
        3.0f,
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
            IntroSndGemNotice2,
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
            IntroSndGemHudGlow,
            0.5f,
            1.0f,
            0.0f,
            10
        }
    },
    {
        16.5f,
        CutsceneEventTypeSeqStop,
    },
    {
        16.5f,
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
        17.5f,
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
        16.5f,
        CutsceneEventTypeSeq,
        .seq = {
            _cosmicDustSegmentRomStart,
            _cosmicDustSegmentRomEnd,
            9600, 0, 0, 0
        }
    },
    {
        20.5f,
        CutsceneEventTypeSoundStop,
        .sound = {IntroSndCockpit}
    },
    {
        20.5f,
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