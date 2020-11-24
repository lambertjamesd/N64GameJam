
#ifndef _LEVEL_LEVEL_H
#define _LEVEL_LEVEL_H

#include "src/collision/levelcollisiongrid.h"
#include "src/graphics/levelgraphics.h"
#include "src/graphics/levelthemegraphics.h"

typedef void (*CleanupFunction)(void*);

#define LEVEL_HAS_CADET         0x1
#define LEVEL_HAS_ROBOT         0x2
#define LEVEL_INTRO_CUTSCENE    0x4
#define LEVEL_EXIT_CUTSCENE     0x8
#define LEVEL_INTRO_ROBOT       0x10
#define LEVEL_FOCUS_CUTSCENE    0x20

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

struct LevelGemDef {
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
    struct LevelGemDef* gems;
    int gemCount;
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

enum LevelPlayMode {
    LevelPlayModeSingle,
    LevelPlayModeCoOp,
};

extern struct LevelDefinition* gLoadedLevel;
extern int gCurrentLevel;
extern int gNextLevel;
extern int gLevelFlags;
extern enum LevelPlayMode gCurrentPlayMode;

void levelLoad(struct LevelDefinition* levelDef, enum LevelPlayMode playMode);
void restartLevel();
void levelSwitchToCadet();
int levelSwitchToRobot();

void levelFocusCutscene(struct Vector3* target, float time, int viewportIndex);

#endif