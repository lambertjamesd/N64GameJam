
#ifndef _COLLISION_GEO_GEO_H
#define _COLLISION_GEO_GEO_H

#include "src/collision/collisiondata.h"
#include "src/collision/levelcollisiongrid.h"

extern struct CollisionCollider _solid_block_collider;

extern struct LevelCollisionTile gCollideTileFloor;
extern struct LevelCollisionTile gCollideTilePlatform;
extern struct LevelCollisionTile gCollideTileBarrier;
extern struct LevelCollisionTile gCollideTileOverhang;
extern struct LevelCollisionTile gCollideTileTunnel;
extern struct LevelCollisionTile gCollideTileRamp;
extern struct LevelCollisionTile gCollideTileStair;
extern struct LevelCollisionTile gCollideTileLava;
extern struct LevelCollisionTile gCollideTileLavaOverhang;

#endif