
#ifndef _COLLISION_COLLISIONSCENE_H
#define _COLLISION_COLLISIONSCENE_H

#include "src/math/vector.h"
#include "src/math/basictransform.h"
#include "collisiondata.h"

#define GLOBAL_GRAVITY -9.8f

struct CollisionResult* collisionSceneCollideSphere(struct Vector3* position, float radius, int collisionMask);

#endif