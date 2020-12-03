
#ifndef _EFFECTS_ROCKETTRAIL_H
#define _EFFECTS_ROCKETTRAIL_H

#include "src/math/basictransform.h"
#include "src/time/time.h"

#define MAX_ACTIVE_PARTICLES    4

struct RocketTrail {
    struct BasicTransform *emitSource;
    struct Vector3 emitOffset;
    struct BasicTransform particleTransforms[MAX_ACTIVE_PARTICLES];
    struct TimeUpdateListener updateListener;
    float spawnTimer;
    int renderId;
    char alpha;
};

void rocektTrailStart(struct RocketTrail* trail, struct BasicTransform *emitSource, struct Vector3* emitOffset);
void rocektTrailStop(struct RocketTrail* trail);

#endif