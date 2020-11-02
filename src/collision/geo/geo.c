
#include "geo.h"

#include "solid_block.inc.c"
#include "tunnel_block.inc.c"
#include "ramp_block.inc.c"
#include "stair_block.inc.c"

struct LevelCollisionTile gCollideTileFloor = {
    {&_solid_block_collider, 0, 0},
};

struct LevelCollisionTile gCollideTilePlatform = {
    {&_solid_block_collider, &_solid_block_collider, 0},
};

struct LevelCollisionTile gCollideTileBarrier = {
    {&_solid_block_collider, &_solid_block_collider, &_solid_block_collider},
};

struct LevelCollisionTile gCollideTileOverhang = {
    {&_solid_block_collider, 0, &_solid_block_collider},
};

struct LevelCollisionTile gCollideTileTunnel = {
    {&_solid_block_collider, &_tunnel_block_collider, 0},
};

struct LevelCollisionTile gCollideTileRamp = {
    {&_solid_block_collider, &_ramp_block_collider, 0},
};

struct LevelCollisionTile gCollideTileStair = {
    {&_solid_block_collider, &_stair_block_collider, 0},
};

struct LevelCollisionTile gCollideTileLava = {
    {&_solid_block_collider, 0, 0},
};

struct LevelCollisionTile gCollideTileLavaOverhang = {
    {&_solid_block_collider, 0, &_solid_block_collider},
};