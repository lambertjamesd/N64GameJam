
#ifndef EFFECTS_SPLASH_H
#define EFFECTS_SPLASH_H

#include "src/math/basictransform.h"
#include "src/time/time.h"

struct SplashEffect {
    struct BasicTransform transform;
    struct TimeUpdateListener updateListener;
    float animationTime;
    float sizeScale;
    int renderId;
};

void splashEffectInit(struct SplashEffect* splash, struct Vector3* at, float sizeScale);
int splashEffectActive(struct SplashEffect* splash);

#endif