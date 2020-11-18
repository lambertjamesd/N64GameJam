
#ifndef EFFECTS_LEVELTITLE_H
#define EFFECTS_LEVELTITLE_H

#include "src/time/time.h"

struct LevelTitleEffect {
    float currTime;
    float titleWidth;
    float edgeAccel;
    char* message;
    struct TimeUpdateListener updateListener;
};

void levelTitleEffectInit(struct LevelTitleEffect* titleEffect, char* message);

extern struct LevelTitleEffect gLevelTitleEffect;

#endif