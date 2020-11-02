
#include "geo.h"

#include "solid_block.inc.c"
#include "tunnel_block.inc.c"
#include "ramp_block.inc.c"
#include "stair_block.inc.c"

struct CollisionCollider gBasicCollisionBlock = {
    ColliderTypeBox,
    0,
    .box = {
        {-1.0f, -2.0f, -1.0f},
        {1.0f, 0.0f, 1.0f},
    }
};

struct LevelCollisionTile gCollideTileFloor = {
    {&gBasicCollisionBlock, 0, 0},
};

struct LevelCollisionTile gCollideTilePlatform = {
    {&gBasicCollisionBlock, &gBasicCollisionBlock, 0},
};

struct LevelCollisionTile gCollideTileBarrier = {
    {&gBasicCollisionBlock, &gBasicCollisionBlock, &gBasicCollisionBlock},
};

struct LevelCollisionTile gCollideTileOverhang = {
    {&gBasicCollisionBlock, 0, &gBasicCollisionBlock},
};

struct LevelCollisionTile gCollideTileTunnel = {
    {&gBasicCollisionBlock, &_tunnel_block_collider, 0},
};

struct LevelCollisionTile gCollideTileRamp = {
    {&gBasicCollisionBlock, &_ramp_block_collider, 0},
};

struct LevelCollisionTile gCollideTileStair = {
    {&gBasicCollisionBlock, &_stair_block_collider, 0},
};

struct LevelCollisionTile gCollideTileLava = {
    {&gBasicCollisionBlock, 0, 0},
};

struct LevelCollisionTile gCollideTileLavaOverhang = {
    {&gBasicCollisionBlock, 0, &gBasicCollisionBlock},
};