
#ifndef _EFFECTS_ROCKETTRAIL_H
#define _EFFECTS_ROCKETTRAIL_H

#include "src/math/basictransform.h"
#include "src/time/time.h"
#include "src/graphics/levelthemegraphics.h"

#define ROCKET_TRAIL_FLAGS_RELATIVE (1 << 0)

struct RocketTrailParameters {
    enum TransparentMaterialType material;
    struct Vector3 emitOffset;
    float moveSpeed;
    float scaleRate;
    float minScale;
    float maxScale;
    float spawnInterval;
};

struct RocketTrail {
    struct BasicTransform *emitSource;
    struct RocketTrailParameters* parameters;
    struct TimeUpdateListener updateListener;
    float spawnTimer;
    int renderId;
    char alpha;
    char flags;
    short particleCount;
    struct BasicTransform particleTransforms[];
};

void rocektTrailStart(struct RocketTrail* trail, struct BasicTransform *emitSource, struct RocketTrailParameters* parameters, short particleCount, char flags);
void rocektTrailStop(struct RocketTrail* trail);

#endif