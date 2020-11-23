
#include "level.h"

#include "src/audio/audio.h"
#include "src/audio/playersounds.h"
#include "src/boot.h"
#include "src/cadet/cadet.h"
#include "src/collision/collisionscene.h"
#include "src/effects/explosion.h"
#include "src/effects/leveltitle.h"
#include "src/effects/shadow.h"
#include "src/effects/tutorial.h"
#include "src/graphics/dynamic.h"
#include "src/graphics/graphics.h"
#include "src/input/controller.h"
#include "src/input/inputfocus.h"
#include "src/levels/levels.h"
#include "src/menu/pausemenu.h"
#include "src/puzzle/breakable.h"
#include "src/puzzle/door.h"
#include "src/puzzle/entranceexit.h"
#include "src/puzzle/gem.h"
#include "src/puzzle/movingplatform.h"
#include "src/puzzle/switch.h"
#include "src/robot/robot.h"
#include "src/save/savefile.h"
#include "src/system/memory.h"
#include "src/time/time.h"

struct LevelDefinition* gLoadedLevel;

struct ExpandedLevel {
    struct Gem* gems;
    int gemCount;
    CleanupFunction* cleanupFns; 
    void** cleanupParams;
    int cleanupCount;
};

struct ExpandedLevel gExpandedLevel;

int gLevelFlags;
struct TimeUpdateListener gLevelUpdateListener;
int gCurrentLevel;
int gNextLevel;
float gLevelCutsceneTimer;

void levelNext() {
    if (gNextLevel == gCurrentLevel && gCurrentLevel + 1 < _level_group_all_levels_count) {
        gNextLevel = gCurrentLevel + 1;
    }
}

void levelPrev() {
    if (gNextLevel == gCurrentLevel && gCurrentLevel > 0) {
        gNextLevel = gCurrentLevel - 1;
    }
}

void restartLevel() {
    gNextLevel = gCurrentLevel;
    gCurrentLevel = -1;
}

void levelSwitchToCadet() {
    gInputMask = INPUT_MASK_CADET;

    audioRestartPlaySound(
        gPlayerSoundIds[PlayerSwitchBack],
        0.5f,
        1.0f,
        0.0f,
        10
    );
}

void levelSwitchToRobot() {
    gInputMask = INPUT_MASK_ROBOT;

    audioRestartPlaySound(
        gPlayerSoundIds[PlayerSwitch],
        0.5f,
        1.0f,
        0.0f,
        10
    );
}

void levelFocusCutscene(struct Vector3* target, float time) {
    gLevelCutsceneTimer = time;
    inputMaskPush(0);
    gLevelFlags |= LEVEL_FOCUS_CUTSCENE;
    gScene.camera[0].targetPosition = *target;
    gScene.camera[0].followDistanceStep = 0;
}

void levelUpdate(void* data) {
    if (gInputMask & InputMaskCadet) {
        gScene.camera[0].targetPosition = gCadet.transform.position;

        if (gScene.camera[0].targetPosition.y < 0.0f) {
            gScene.camera[0].targetPosition.y = 0.0f;
        }

        if (getButtonDown(0, L_TRIG | Z_TRIG) && (gLevelFlags & LEVEL_HAS_ROBOT)) {
            levelSwitchToRobot();
        }
    } else if (gInputMask & InputMaskRobot) {
        gScene.camera[0].targetPosition = gRobot.transform.position;

        if (gScene.camera[0].targetPosition.y < 0.0f) {
            gScene.camera[0].targetPosition.y = 0.0f;
        }

        if (getButtonDown(0, L_TRIG | Z_TRIG) && (gLevelFlags & LEVEL_HAS_CADET)) {
            levelSwitchToCadet();
        }
    }

    if ((gInputMask & (InputMaskCadet | InputMaskRobot)) && 
        !(gLevelFlags & (LEVEL_INTRO_CUTSCENE | LEVEL_FOCUS_CUTSCENE))) {
        tutorialMenuCheck();
    }

    if (gLevelFlags & LEVEL_INTRO_CUTSCENE) {
        if (!(gCadet.actor.stateFlags & CADET_IS_CUTSCENE)) {
            gLevelFlags &= ~LEVEL_INTRO_CUTSCENE;
            gInputMask = INPUT_MASK_CADET;
            gScene.camera[0].followDistanceStep = 1;
        }
    }

    if (gLevelFlags & LEVEL_FOCUS_CUTSCENE) {
        gLevelCutsceneTimer -= gTimeDelta;

        if (gLevelCutsceneTimer < 0.0f) {
            gLevelFlags &= ~LEVEL_FOCUS_CUTSCENE;
            inputMaskPop();
            gScene.camera[0].followDistanceStep = 1;
        }
    }

    if (!(gLevelFlags & (LEVEL_INTRO_CUTSCENE | LEVEL_FOCUS_CUTSCENE)) && getButtonDown(0, START_BUTTON)) {
        pauseMenuShow(&gPauseMenu);
    }

    if ((!(gLevelFlags & LEVEL_HAS_CADET) || gCadetExit.isActive) && (!(gLevelFlags & LEVEL_HAS_ROBOT) || gRobotExit.isActive)) {
        cadetFinishLevel(&gCadet);
        robotFinishLevel(&gRobot);
        gScene.camera[0].followDistanceStep = 0;
        gLevelFlags |= LEVEL_EXIT_CUTSCENE;
        gInputMask = 0;

        int i;

        for (i = 0; i < gExpandedLevel.gemCount; ++i) {
            gemEndCutscene(&gExpandedLevel.gems[i], &gCadet.transform.position);
        }

        enum PlayerSounds exitSound = PlayerSoundsBothWarp;

        if (!(gLevelFlags & LEVEL_HAS_ROBOT)) {
            exitSound = PlayerSoundsCadetWarp;
        } else if (!(gLevelFlags & LEVEL_HAS_CADET)) {
            exitSound = PlayerSoundsRobotWarp;
        }

        audioPlaySound(
            gPlayerSoundIds[exitSound],
            0.5f,
            1.0f,
            0.0f,
            10
        );
    }

    if (gLevelFlags & LEVEL_EXIT_CUTSCENE) {
        if (gCadet.actor.stateFlags & CADET_IS_INVISIBLE) {
            levelNext();
            gLevelFlags &= ~LEVEL_EXIT_CUTSCENE;
        }
    }
    
    if (getButtonDown(0, R_JPAD)) {
        levelNext();
    } else if (getButtonDown(0, L_JPAD)) {
        levelPrev();
    }
}

void levelExpand(struct LevelDefinition* levelDef) {
    /////////////////////
    // switches

    struct PuzzleSwitch* switches = heapMalloc(
        ARRAY_SIZE(struct PuzzleSwitch, levelDef->levelData->switchCount), 
        ALIGNMENT_OF(struct PuzzleSwitch)
    );

    int i;
    for (i = 0; i < levelDef->levelData->switchCount; ++i) {
        struct LevelSwitchDef* def = &levelDef->levelData->switches[i];
        switchInit(&switches[i], &def->pos, def->type, def->color);
    }

    /////////////////////
    // doors
    
    struct PuzzleDoor* doors = heapMalloc(
        ARRAY_SIZE(struct PuzzleDoor, levelDef->levelData->doorCount), 
        ALIGNMENT_OF(struct PuzzleDoor)
    );

    for (i = 0; i < levelDef->levelData->doorCount; ++i) {
        struct LevelDoorDef* def = &levelDef->levelData->doors[i];
        doorInit(&doors[i], &def->pos, def->color, def->inverted);
    }

    /////////////////////
    // barriers

    struct Breakable* breakables = heapMalloc(
        ARRAY_SIZE(struct Breakable, levelDef->levelData->breakableCount),
        ALIGNMENT_OF(struct Breakable)
    );

    for (i = 0; i < levelDef->levelData->breakableCount; ++i) {
        struct LevelBreakableDef* def = &levelDef->levelData->breakables[i];
        breakableInit(&breakables[i], &def->pos, def->type);
    }

    /////////////////////
    // platform slots

    struct MovingPlatformSlot* slots = heapMalloc(
        ARRAY_SIZE(struct MovingPlatformSlot, levelDef->levelData->platformSlotCount),
        ALIGNMENT_OF(struct MovingPlatformSlot)
    );

    for (i = 0; i < levelDef->levelData->platformSlotCount; ++i) {
        movingPlatformSlotInit(&slots[i], &levelDef->levelData->platformSlots[i].pos);
    }
    
    movingPlatformJoinSlots(slots, levelDef->levelData->platformSlotCount);

    /////////////////////
    // platform slots

    struct MovingPlatform* platforms = heapMalloc(
        ARRAY_SIZE(struct MovingPlatform, levelDef->levelData->platformCount),
        ALIGNMENT_OF(struct MovingPlatform)
    );

    for (i = 0; i < levelDef->levelData->platformCount; ++i) {
        struct LevelPlatformDef* platform = &levelDef->levelData->platforms[i];
        movingPlatformInit(&platforms[i], &platform->pos, &slots[platform->slotIndex], platform->color);
    }

    /////////////////////
    // gems

    struct Gem* gems = heapMalloc(
        ARRAY_SIZE(struct Gem, levelDef->levelData->gemCount),
        ALIGNMENT_OF(struct Gem)
    );

    for (i = 0; i < levelDef->levelData->gemCount; ++i) {
        struct LevelGemDef* gem = &levelDef->levelData->gems[i];
        gemInit(&gems[i], &gem->pos, i);
    }

    gExpandedLevel.gems = gems;
    gExpandedLevel.gemCount = levelDef->levelData->gemCount;

    /////////////////////
    // cleanup
    
    gExpandedLevel.cleanupFns = 0;
    gExpandedLevel.cleanupParams = 0;
    gExpandedLevel.cleanupCount = 0;
}

void levelCleanup(struct LevelDefinition* levelDef) {
    int i;

    for (i = 0; i < gExpandedLevel.cleanupCount; ++i) {
        gExpandedLevel.cleanupFns[i](gExpandedLevel.cleanupParams[i]);
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

    timeResetListeners();
    signalResetAll();
    graphicsClearMenus();

    graphicsInitLevel(
        staticSegment, 
        levelSegment, 
        themeSegment, 
        levelDef->levelData->graphics, 
        levelDef->theme->theme
    );

    renderSceneReset(&gScene);

    collisionSceneUseGrid(levelDef->levelData->collision);
    
    gLevelFlags = LEVEL_INTRO_CUTSCENE;

    gInputMask = INPUT_MASK_CADET;
    gInputMask = 0;

    gLevelFlags |= LEVEL_HAS_CADET;
    gScene.activeViewportCount = 2;
    cameraInit(&gScene.camera[0], &levelDef->levelData->cadetStart);
    cameraInit(&gScene.camera[1], &levelDef->levelData->robotStart);

    gScene.viewports[0].minx = 0;
    gScene.viewports[0].maxx = SCREEN_WD/2-1;
    gScene.viewports[0].miny = 0;
    gScene.viewports[0].maxy = SCREEN_HT;

    gScene.viewports[1].minx = SCREEN_WD/2+1;
    gScene.viewports[1].maxx = SCREEN_WD;
    gScene.viewports[1].miny = 0;
    gScene.viewports[1].maxy = SCREEN_HT;

    cadetReset(&levelDef->levelData->cadetStart);
    entranceExitInit(&gCadetExit, &levelDef->levelData->cadetFinish, 1);

    if (levelDef->levelData->robotStart.z < 0.5 && levelDef->levelData->robotStart.x > -0.5) {
        if (gSaveFile.tutorialFlags & SAVEFILE_LEARNED_FOUND_ROBOT) {
            gLevelFlags |= LEVEL_HAS_ROBOT;
        } else {
            gLevelFlags |= LEVEL_INTRO_ROBOT;
        }

        robotReset(&levelDef->levelData->robotStart);
        entranceExitInit(&gRobotExit, &levelDef->levelData->robotFinish, 0);
    }

    levelExpand(levelDef);
    timeAddListener(&gLevelUpdateListener, levelUpdate, 0, TimeUpdateGroupWorld);

    gLoadedLevel = levelDef;

    gScene.transparentMaterials[TransparentMaterialTypeShadow] = _drop_shadow_material;
    gScene.transparentMaterials[TransparentMaterialTypeGem] = _gem_mat;
    gScene.transparentMaterialCleanup[TransparentMaterialTypeGem] = _gem_cleanup_mat;
    gScene.transparentMaterials[TransparentMaterialTypeShockwave] = _shockwave_mat;

    levelTitleEffectInit(&gLevelTitleEffect, levelDef->levelData->name);
}