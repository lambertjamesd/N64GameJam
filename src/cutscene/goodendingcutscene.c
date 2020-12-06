
#include "goodendingcutscene.h"
#include "src/audio/allseq.h"

struct CutsceneFrame gGoodEndingCutsceneFrames[] = {
    {3.0f, _00_good_ending, 0.0f, 0.0f},
    {4.0f, _00_good_ending_closeup, 0.0f, 0.0f},
    {4.0f, _01_see_robot, 0.0f, 0.0f},
    {6.0f, _02_leave_with_robot, 0.0f, 0.0f, -53.333333333f, 320.0f},
    {6.0f, _02_leave_with_robot, 0.0f, 0.0f},
    {5.0f, _10_credits_text, 0.0f, 0.0f},
    {5.0f, _11_homebrew, 0.0f, 0.0f},
};

struct CutsceneEvent gGoodEndingCutsceneEvents[] = {
    {
        0.0f,
        CutsceneEventTypeSeq,
        .seq = {
            _cosmicDustSegmentRomStart,
            _cosmicDustSegmentRomEnd,
            0, 0, 0, 0,
            0x4000,
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
        12.0f,
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

struct Cutscene gGoodEndingCutscene = {
    gGoodEndingCutsceneFrames,
    sizeof(gGoodEndingCutsceneFrames)/sizeof(*gGoodEndingCutsceneFrames),
    gGoodEndingCutsceneEvents,
    sizeof(gGoodEndingCutsceneEvents)/sizeof(*gGoodEndingCutsceneEvents),
    SoundBankIntroCutscene,
};