
#ifndef _COLLISOIN_SPHEREACTOR_H
#define _COLLISOIN_SPHEREACTOR_H

#include "collisionscene.h"

#define SPHERE_ACTOR_IS_GROUNDED  (1 << 0)

struct SphereActor {
    struct Vector3 velocity;
    struct Vector3 lastStableLocation;
    struct BasicTransform* anchor;
    struct Vector3 relativeToAnchor;
    float radius;
    int stateFlags;
    int collisionMask;
};

void sphereActorCollideScene(struct SphereActor* actor, struct Vector3* position);

#endif