
#ifndef _COLLISION_COLLISIONSCENE_H
#define _COLLISION_COLLISIONSCENE_H

#include "src/math/vector.h"
#include "src/math/basictransform.h"
#include "collisiondata.h"
#include "levelcollisiongrid.h"

#define GLOBAL_GRAVITY -9.8f
#define KILL_PLANE_HEIGHT -4.0f

struct CollisionResult* collisionSceneCollideSphere(struct Vector3* position, float radius, int collisionMask);

void collisionSceneUseGrid(struct LevelCollisionGrid* grid);

#endif