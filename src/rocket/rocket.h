
#ifndef _ROCKET_ROCKET_H
#define _ROCKET_ROCKET_H

#include "src/math/basictransform.h"
#include "src/graphics/dynamic.h"
#include "src/graphics/color.h"
#include "src/time/time.h"

struct Rocket {
    struct BasicTransform transform;
    struct TimeUpdateListener updateListener;
    struct Color color;
    struct Vector3 landingSpot;
    float animationTiming;
};

extern struct Rocket gRocket;

void rocketRender(struct DynamicActor* data, struct GraphicsState* state);
void rocketLandAt(struct Rocket* rocket, struct Vector3* location);

#endif