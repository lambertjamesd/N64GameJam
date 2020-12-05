
#ifndef _ROCKET_ROCKET_H
#define _ROCKET_ROCKET_H

#include "src/math/basictransform.h"
#include "src/graphics/dynamic.h"
#include "src/graphics/color.h"
#include "src/time/time.h"
#include "src/collision/sparsecollisiongrid.h"
#include "src/effects/rockettrail.h"

#define ROCKET_FLAGS_LAUNCHING      (1 << 0)
#define ROCKET_FLAGS_ANIMATION_DONE (1 << 1)

extern struct RocketTrailParameters gRocketTrailParameters;
#define ROCKET_TRAIL_PARAMETER_COUNT       4

struct Rocket {
    struct BasicTransform transform;
    struct TimeUpdateListener updateListener;
    struct CollisionTransformedCollider collider;
    struct Color color;
    struct Vector3 landingSpot;
    float animationTiming;
    int rocketFlags;
    struct RocketTrail trail;
    struct BasicTransform trailParticles[ROCKET_TRAIL_PARAMETER_COUNT];
};

extern struct Rocket gRocket;

void rocketRender(struct DynamicActor* data, struct GraphicsState* state);
void rocketLandAt(struct Rocket* rocket, struct Vector3* location);
void rocketStartAt(struct Rocket* rocket, struct Vector3* location);
void rocketLaunch(struct Rocket* rocket);

#endif