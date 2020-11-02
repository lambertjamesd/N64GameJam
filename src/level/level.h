
#ifndef _LEVEL_LEVEL_H
#define _LEVEL_LEVEL_H

#include "src/collision/levelcollisiongrid.h"
#include "src/graphics/levelgraphics.h"

struct LevelData {
    struct LevelGraphics* graphics;
    struct LevelCollisionGrid* collision;
};

#endif