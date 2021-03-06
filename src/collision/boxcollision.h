
#ifndef _COLLISION_BOX_COLLISION
#define _COLLISION_BOX_COLLISION

#include "collisiondata.h"

int collisionBoxCollideSphere(struct Box* box, struct Vector3* center, float radius, struct CollisionResult* result);
int collisionBoxOverlapSphere(struct Box* box, struct Vector3* center, float radius);

float collisionBoxRaycast(struct Box* mesh, struct Vector3* origin, struct Vector3* dir, struct ContactPoint* contact);

#endif