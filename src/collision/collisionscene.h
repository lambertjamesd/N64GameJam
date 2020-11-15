
#ifndef _COLLISION_COLLISIONSCENE_H
#define _COLLISION_COLLISIONSCENE_H

#include "src/math/vector.h"
#include "src/math/basictransform.h"
#include "collisiondata.h"
#include "levelcollisiongrid.h"
#include "sparsecollisiongrid.h"

#define GLOBAL_GRAVITY -18.8f
#define KILL_PLANE_HEIGHT -4.0f

extern struct SparseCollisionGrid gSparseCollisionGrid;

struct CollisionResult* collisionSceneCollideSphere(struct Vector3* position, float radius, int collisionMask);
float collisionSceneRaycast(struct Vector3* position, struct Vector3* dir, int collisionMask, float maxDistance, struct ContactPoint* hit);

void collisionSceneUseGrid(struct LevelCollisionGrid* grid);

#endif