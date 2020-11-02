
#include "geo.h"

#include "solid_block.inc.c"

struct LevelCollisionTile gCollideTileFloor = {
    {&_solid_block_collider, 0, 0},
};