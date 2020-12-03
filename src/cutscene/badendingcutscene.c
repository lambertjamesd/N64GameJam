
#include "badendingcutscene.h"
#include "src/audio/allseq.h"

struct CutsceneFrame gBadEndingCutsceneFrames[] = {
    {3.0f, _00_bad_ending, 0.0f, 0.0f},
    {5.0f, _00_bad_ending_closeup, 0.0f, 0.0f},
    {4.0f, _01_see_robot, 0.0f, 0.0f},
    {4.0f, _02_sad_cadet, 0.0f, 0.0f},
    {4.0f, _03_leave_robot, 0.0f, 0.0f},
    {6.0f, _04_leave_planet, 0.0f, 0.0f},
    {5.0f, _10_credits_text, 0.0f, 0.0f},
    {5.0f, _11_homebrew, 0.0f, 0.0f},
};


struct CutsceneEvent gBadEndingCutsceneEvents[] = {
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
        5.0f,
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
        16.0f,
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

struct Cutscene gBadEndingCutscene = {
    gBadEndingCutsceneFrames,
    sizeof(gBadEndingCutsceneFrames)/sizeof(*gBadEndingCutsceneFrames),
    gBadEndingCutsceneEvents,
    sizeof(gBadEndingCutsceneEvents)/sizeof(*gBadEndingCutsceneEvents),
    SoundBankIntroCutscene,
};