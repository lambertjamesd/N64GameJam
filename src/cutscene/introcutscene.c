
#include "introcutscene.h"

struct CutsceneFrame gIntroCutsceneFrames[] = {
    {3.0f, 0, 0.0f, 0.0f},
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

struct Cutscene gIntroCutscene = {
    gIntroCutsceneFrames,
    sizeof(gIntroCutsceneFrames)/sizeof(*gIntroCutsceneFrames),
};