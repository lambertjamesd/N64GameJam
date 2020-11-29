
#include "goodendingcutscene.h"

struct CutsceneFrame gGoodEndingCutsceneFrames[] = {
    {6.0f, 14, 0.0f, 0.0f},
    {4.0f, 10, 0.0f, 0.0f},
    {12.0f, 15, 0.0f, 0.0f},
};

struct Cutscene gGoodEndingCutscene = {
    gGoodEndingCutsceneFrames,
    sizeof(gGoodEndingCutsceneFrames)/sizeof(*gGoodEndingCutsceneFrames),
};