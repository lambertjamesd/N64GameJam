
#include "level.h"

#include "src/boot.h"
#include "src/collision/collisionscene.h"
#include "src/graphics/graphics.h"
#include "src/graphics/dynamic.h"
#include "src/system/memory.h"
#include "src/cadet/cadet.h"
#include "src/robot/robot.h"
#include "src/puzzle/switch.h"

struct LevelDefinition* gLoadedLevel;

static CleanupFunction* gCleanupFn; 
static void** gCleanupParam;
static int gCleanupCount;

void levelExpand(struct LevelDefinition* levelDef) {
    struct PuzzleSwitch* switches = fastMalloc(ARRAY_SIZE(struct PuzzleSwitch, levelDef->levelData->switchCount), ALIGNMENT_OF(struct PuzzleSwitch));

    int i;
    for (i = 0; i < levelDef->levelData->switchCount; ++i) {
        struct LevelSwitchDef* def = &levelDef->levelData->switches[i];
        switchInit(&switches[i], &def->pos, def->type, def->color);
    }

    gCleanupCount = levelDef->levelData->switchCount;

    gCleanupFn = fastMalloc(ARRAY_SIZE(
        CleanupFunction,
        gCleanupCount
    ), ALIGNMENT_OF(CleanupFunction));

    gCleanupParam = fastMalloc(ARRAY_SIZE(
        void*,
        gCleanupCount
    ), ALIGNMENT_OF(void*));

    int cleanupIndex = 0;

    for (i = 0; i < levelDef->levelData->switchCount; ++i) {   
        gCleanupFn[i] = switchDestroy;
        gCleanupParam[i] = &switches[i];
        ++cleanupIndex;
    }
}

void levelCleanup(struct LevelDefinition* levelDef) {
    int i;

    for (i = 0; i < gCleanupCount; ++i) {
        gCleanupFn[i](gCleanupParam[i]);
    }
}

void levelLoad(struct LevelDefinition* levelDef) {
    if (gLoadedLevel) {
        levelCleanup(gLoadedLevel);
    }

    heapReset();
    osUnmapTLBAll();

    u32 len = (u32)(_staticSegmentRomEnd - _staticSegmentRomStart);
    char* staticSegment = heapMalloc(len, 8);
    romCopy(_staticSegmentRomStart, (char*)staticSegment, len);

    len = (u32)(levelDef->levelRomEnd - levelDef->levelRomStart);
    u32 levelAlign;
    u32 levelPageMask;
    tlbAlign(len, &levelAlign, &levelPageMask);

    char* levelSegment = heapMalloc(len, levelAlign);
    romCopy(levelDef->levelRomStart, levelSegment, len);
    osMapTLB(0, levelPageMask, (void*)(LEVEL_SEGMENT << 24), osVirtualToPhysical(levelSegment), -1, -1);


    len = (u32)(levelDef->theme->themeRomEnd - levelDef->theme->themeRomStart);
    char* themeSegment = heapMalloc(len, 8);
    romCopy(levelDef->theme->themeRomStart, (char*)themeSegment, len);

    graphicsInitLevel(
        staticSegment, 
        levelSegment, 
        themeSegment, 
        levelDef->levelData->graphics, 
        levelDef->theme->theme
    );

    dynamicActorGroupReset(&gScene.dynamicActors);

    collisionSceneUseGrid(levelDef->levelData->collision);
    
    cadetReset(&levelDef->levelData->cadetStart);
    robotReset(&levelDef->levelData->robotStart);

    levelExpand(levelDef);

    gLoadedLevel = levelDef;
}