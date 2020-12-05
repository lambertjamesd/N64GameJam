
#ifndef _EFFECTS_ROCKETTRAIL_H
#define _EFFECTS_ROCKETTRAIL_H

#include "src/math/basictransform.h"
#include "src/time/time.h"
#include "src/graphics/levelthemegraphics.h"

#define MAX_ACTIVE_PARTICLES    4

#define ROCKET_TRAIL_FLAGS_RELATIVE (1 << 0)

struct RocketTrailParameters {
    enum TransparentMaterialType material;
    struct Vector3 emitOffset;
};

struct RocketTrail {
    struct BasicTransform *emitSource;
    struct RocketTrailParameters* parameters;
    struct BasicTransform particleTransforms[MAX_ACTIVE_PARTICLES];
    struct TimeUpdateListener updateListener;
    float spawnTimer;
    int renderId;
    char alpha;
    char flags;
};

void rocektTrailStart(struct RocketTrail* trail, struct BasicTransform *emitSource, struct RocketTrailParameters* parameters, char flags);
void rocektTrailStop(struct RocketTrail* trail);

#endif