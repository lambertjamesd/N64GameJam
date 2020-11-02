
#ifndef _COLLISION_BOX_COLLISION
#define _COLLISION_BOX_COLLISION

#include "collisiondata.h"

int collisionBoxCollideSphere(struct CollisionBox* box, struct Vector3* center, float radius, struct CollisionResult* result);

#endif