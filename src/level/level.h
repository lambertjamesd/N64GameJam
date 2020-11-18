
#ifndef _LEVEL_LEVEL_H
#define _LEVEL_LEVEL_H

#include "src/collision/levelcollisiongrid.h"
#include "src/graphics/levelgraphics.h"
#include "src/graphics/levelthemegraphics.h"

typedef void (*CleanupFunction)(void*);

#define LEVEL_HAS_CADET 1
#define LEVEL_HAS_ROBOT 2
#define LEVEL_INTRO_CUTSCENE 4
#define LEVEL_EXIT_CUTSCENE 8

struct LevelSwitchDef {
    struct Vector3 pos;
    short type;
    short color;
};

struct LevelDoorDef {
    struct Vector3 pos;
    short color;
    short inverted;
};

struct LevelBreakableDef {
    struct Vector3 pos;
    short type;
};

struct LevelPlatformDef {
    struct Vector3 pos;
    short color;
    short slotIndex;
};

struct LevelPlatformSlotDef {
    struct Vector3 pos;
};

struct LevelData {
    struct LevelGraphics* graphics;
    struct LevelCollisionGrid* collision;
    struct Vector3 cadetStart;
    struct Vector3 robotStart;
    struct Vector3 cadetFinish;
    struct Vector3 robotFinish;
    struct LevelSwitchDef* switches;
    int switchCount;
    struct LevelDoorDef* doors;
    int doorCount;
    struct LevelBreakableDef* breakables;
    int breakableCount;
    struct LevelPlatformDef* platforms;
    int platformCount;
    struct LevelPlatformSlotDef* platformSlots;
    int platformSlotCount;
    char* name;
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

extern struct LevelDefinition* gLoadedLevel;
extern int gCurrentLevel;
extern int gNextLevel;
extern int gLevelFlags;

void levelLoad(struct LevelDefinition* levelDef);

#endif