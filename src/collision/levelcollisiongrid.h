
#ifndef _COLLISION_LEVELCOLLISION_H
#define _COLLISION_LEVELCOLLISION_H

#include "collisiondata.h"

#define LEVEL_BLOCK_HEIGHT  3
#define LEVEL_GRID_SIZE     2

struct LevelCollisionTile {
    struct CollisionCollider* blocks[LEVEL_BLOCK_HEIGHT];
};

struct LevelCollisionCell {
    struct LevelCollisionTile* tile;
    int rotation;
};

struct LevelCollisionGrid {
    // struct LevelCollisionCell tiles[x * height + z]
    struct LevelCollisionCell* tiles;
    int width;
    int height;
};

int collisionGridCollideSphere(struct Vector3* center, float radius, struct LevelCollisionGrid* grid, int collisionMask, struct CollisionResult* result);

#endif