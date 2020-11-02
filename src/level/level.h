
#ifndef _LEVEL_LEVEL_H
#define _LEVEL_LEVEL_H

#include "src/collision/levelcollisiongrid.h"
#include "src/graphics/levelgraphics.h"
#include "src/graphics/levelthemegraphics.h"

struct LevelData {
    struct LevelGraphics* graphics;
    struct LevelCollisionGrid* collision;
    struct Vector3 cadetStart;
    struct Vector3 robotStart;
};

struct LevelThemeDefinition {
    char* themeRomStart;
    char* themeRomEnd;
    struct LevelThemeGraphics* theme;
};

struct LevelDefinition {
    char* levelRomStart;
    char* levelRomEnd;
    struct LevelData* levelData;
    struct LevelThemeDefinition* theme;
};

void loadLevel(struct LevelDefinition* levelDef);

#endif