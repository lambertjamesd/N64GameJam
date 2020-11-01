
#ifndef _COLLISION_COLLISIONSCENE_H
#define _COLLISION_COLLISIONSCENE_H

#include "src/math/vector.h"
#include "src/math/basictransform.h"

struct ContactPoint {
    struct Vector3 point;
    struct Vector3 normal;
    struct BasicTransform* transform;
    int collisionMask;
};

struct CollisionResult {
    struct ContactPoint* contacts;
    int contactCount;
};

struct CollisionResult* collisionSceneCollideSphere(struct Vector3* position, float radius, int collisionMask);

#endif