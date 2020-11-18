
#ifndef _COLLISOIN_SPHEREACTOR_H
#define _COLLISOIN_SPHEREACTOR_H

#include "collisionscene.h"

#define SPHERE_ACTOR_IS_GROUNDED    (1 << 0)
#define SPHERE_ACTOR_FLAG_0         (1 << 1)
#define SPHERE_ACTOR_FLAG_1         (1 << 2)
#define SPHERE_ACTOR_FLAG_2         (1 << 3)
#define SPHERE_ACTOR_FLAG_3         (1 << 4)

enum SphereActorCollideResult {
    SphereActorCollideResultNone,
    SphereActorCollideResultKill,
};

struct SphereActor {
    struct Vector3 velocity;
    struct Vector3 lastStableLocation;
    struct BasicTransform* lastStableAnchor;
    struct BasicTransform* anchor;
    struct Vector3 relativeToAnchor;
    int groundCollisionMask;
    float radius;
    int stateFlags;
    int collisionMask;
};

enum SphereActorCollideResult sphereActorCollideScene(struct SphereActor* actor, struct Vector3* position);
void sphereActorQueryScene(struct SphereActor* actor, struct Vector3* position);

#endif