
#include "badendingcutscene.h"

struct CutsceneFrame gBadEndingCutsceneFrames[] = {
    {6.0f, 9, 0.0f, 0.0f},
    {4.0f, 10, 0.0f, 0.0f},
    {4.0f, 11, 0.0f, 0.0f},
    {4.0f, 12, 0.0f, 0.0f},
    {6.0f, 13, 0.0f, 0.0f},
};

struct Cutscene gBadEndingCutscene = {
    gBadEndingCutsceneFrames,
    sizeof(gBadEndingCutsceneFrames)/sizeof(*gBadEndingCutsceneFrames),
};