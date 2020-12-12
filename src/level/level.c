
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
#include "src/rocket/rocket.h"
#include "src/save/savefile.h"
#include "src/system/memory.h"
#include "src/time/time.h"
#include "src/defs.h"
#include "src/font/fontrenderer.h"
#include "src/font/endlessbossbattle/endlessbossbattle.h"
#include "src/font/buttons/buttons.h"

struct LevelDefinition* gLoadedLevel;

struct ExpandedLevel {
    struct Gem* gems;
    int gemCount;
    CleanupFunction* cleanupFns; 
    void** cleanupParams;
    int cleanupCount;
};

struct ExpandedLevel gExpandedLevel;

#define FADE_DURATION       0.5f

int gLevelFlags;
struct TimeUpdateListener gLevelUpdateListener;
int gCurrentLevel;
int gNextLevel;
float gLevelCutsceneTimer;
enum LevelPlayMode gCurrentPlayMode;
short gFocusCamera;
float gFadeTimer = 0.0f;

void levelNext() {
    if (gNextLevel == gCurrentLevel && gCurrentLevel + 1 < _level_group_all_levels_count) {
        levelSetNext(gCurrentLevel + 1, 0);
    }
}

void levelPrev() {
    if (gNextLevel == gCurrentLevel && gCurrentLevel > 0) {
        levelSetNext(gCurrentLevel - 1, 0);
    }
}

void restartLevel() {
    levelSetNext(gCurrentLevel, 0);
    gCurrentLevel = -1;
}

void levelSwitchToCadet() {
    if (gCurrentPlayMode == LevelPlayModeSingle) {
        gCadet.controllerIndex = 0;
        gRobot.controllerIndex = -1;
    }

    audioRestartPlaySound(
        gPlayerSoundIds[PlayerSwitchBack],
        0.5f,
        0.7f,
        0.0f,
        10
    );
}

int levelSwitchToRobot() {
    if (gCurrentPlayMode == LevelPlayModeCoOp) {
        renderSceneSetTargetViewportSplit(SCREEN_WD/2);
        gRobot.controllerIndex = 1;
    } else {
        gCadet.controllerIndex = -1;
        gRobot.controllerIndex = 0;
    }

    audioRestartPlaySound(
        gPlayerSoundIds[PlayerSwitch],
        0.5f,
        0.7f,
        0.0f,
        10
    );

    return gRobot.controllerIndex;
}

char* gCamHudText[] = {
    "Move",
    "View",
    "Back",
};

char gCamHudIcon[] = {
    ButtonFontMappingJoystick,
    ButtonFontMappingC_U,
    ButtonFontMappingR,
};

#define CAM_HUD_PADDING     8

void levelHudRender(void* data, struct GraphicsState* state, struct FontRenderer* fontRenderer) {
    if (gFadeTimer > 0) {
        float fadeLerp = gFadeTimer / FADE_DURATION;

        gDPPipeSync(state->dl++);
	    gDPSetRenderMode(state->dl++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
        gDPSetCombineLERP(state->dl++, 0, 0, 0, 0, 0, 0, 0, ENVIRONMENT, 0, 0, 0, 0, 0, 0, 0, ENVIRONMENT);
        gDPSetEnvColor(state->dl++, 0, 0, 0, (u8)(fadeLerp * 255.0f));
        gDPFillRectangle(
            state->dl++, 
            0, 
            0,
            SCREEN_WD,
            gScreenHeight
        );
        gDPPipeSync(state->dl++);
    }

    int i;
    for (i = 0; i < 2; ++i) {
        if (gInputMask[i] & InputMaskFreeCamera) {
            fontRendererSetScale(fontRenderer, 1.0f, gScreenYScale);

            int centerScreen = (gScene.viewports[i].minx + gScene.viewports[i].maxx) / 2;
            int yScreen = SCALE_FOR_PAL(180);

            int viewWidth = 4 * CAM_HUD_PADDING;
            int itemIndex;

            for (itemIndex = 0; itemIndex < 3; ++itemIndex) {
                viewWidth += (int)fontRendererMeasureWidth(&gEndlessBossBattle, gCamHudText[itemIndex]);
            }

            gDPPipeSync(state->dl++);
	        gDPSetCombineLERP(state->dl++, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1);

            int x = centerScreen - (viewWidth >> 1) + CAM_HUD_PADDING;

            gDPFillRectangle(
                state->dl++, 
                x - CAM_HUD_PADDING, 
                yScreen - SCALE_FOR_PAL(6),
                x - CAM_HUD_PADDING + viewWidth, 
                yScreen + SCALE_FOR_PAL(30)
            );
            gDPPipeSync(state->dl++);

            gSPDisplayList(state->dl++, gEndlessBossBattleUse);
            for (itemIndex = 0; itemIndex < 3; ++itemIndex) {
                fontRendererDrawCharacters(
                    fontRenderer,
                    &gEndlessBossBattle,
                    &state->dl,
                    gCamHudText[itemIndex],
                    x,
                    yScreen
                );

                x += (int)fontRendererMeasureWidth(&gEndlessBossBattle, gCamHudText[itemIndex]) + CAM_HUD_PADDING;
            }

            gDPPipeSync(state->dl++);
            gSPDisplayList(state->dl++, gButtonFontUse);
            x = centerScreen - (viewWidth >> 1) + CAM_HUD_PADDING;
            
            for (itemIndex = 0; itemIndex < 3; ++itemIndex) {
                int textWidth = (int)fontRendererMeasureWidth(&gEndlessBossBattle, gCamHudText[itemIndex]);
                char text[4];
                text[0] = gCamHudIcon[itemIndex];
                text[1] = 0;
                fontRendererDrawCharacters(
                    fontRenderer,
                    &gButtonFont,
                    &state->dl,
                    text,
                    x + (textWidth >> 1) - 5,
                    yScreen + SCALE_FOR_PAL(13)
                );

                x += textWidth + CAM_HUD_PADDING;
            }
        }
    }
    gDPPipeSync(state->dl++);

#if DEBUG || 1

    char buffer[16];
    sprintf(buffer, "%d %d", gUnusedDL, gUnusedMatrices);
    gSPDisplayList(state->dl++, gEndlessBossBattleUse);
    fontRendererSetScale(fontRenderer, 2.0f, 2.0f * gScreenYScale);

    fontRendererDrawCharacters(
        fontRenderer,
        &gEndlessBossBattle,
        &state->dl,
        buffer,
        30,
        30
    );

#endif
    gDPPipeSync(state->dl++);
}

void levelFocusCutscene(struct Vector3* target, float time, int viewportIndex) {
    gLevelCutsceneTimer = time;
    inputMaskPush(0);
    gLevelFlags |= LEVEL_FOCUS_CUTSCENE;

    gFocusCamera = viewportIndex;
    gScene.camera[viewportIndex].targetPosition = *target;
    gScene.camera[viewportIndex].followDistanceStep = 0;
}

void levelUpdateCamera(int controllerIndex, struct Vector3* target) {
    if (gInputMask[controllerIndex] & InputMaskPlayer) {
        gScene.camera[controllerIndex].targetPosition = *target;
        if (gScene.camera[controllerIndex].targetPosition.y < 0.0f) {
            gScene.camera[controllerIndex].targetPosition.y = 0.0f;
        }

        if (getButtonDown(controllerIndex, R_TRIG)) {
            gInputMask[controllerIndex] = INPUT_MASK_FREE_CAM;
            gScene.camera[controllerIndex].followDistanceStep = 2;
            audioRestartPlaySound(
                gPlayerSoundIds[SoundZoomOut],
                0.5f,
                0.5f,
                0.0f, 
                10
            );
        }
    } else if (gInputMask[controllerIndex] & InputMaskFreeCamera) {
        if (getButtonDown(controllerIndex, R_TRIG | B_BUTTON)) {
            gInputMask[controllerIndex] = INPUT_MASK_PLAY;
            gScene.camera[controllerIndex].followDistanceStep = 1;
            audioRestartPlaySound(
                gPlayerSoundIds[SoundZoomIn],
                0.5f,
                0.5f,
                0.0f, 
                10
            );
        }
    }
}

void levelUpdate(void* data) {
    if (gFadeTimer > 0.0f) {
        gFadeTimer -= gTimeDelta;

        if (gFadeTimer < 0.0f) {
            gFadeTimer = 0.0f;
        }
    }

    if ((gInputMask[0] & InputMaskPlayer) && 
        !(gLevelFlags & (LEVEL_INTRO_CUTSCENE | LEVEL_FOCUS_CUTSCENE))) {
        tutorialMenuCheck();
    }

    if (gCurrentPlayMode == LevelPlayModeCoOp) {
        levelUpdateCamera(0, &gCadet.transform.position);
        levelUpdateCamera(1, &gRobot.transform.position);
    } else {
        if (gCadet.controllerIndex != -1) {
            levelUpdateCamera(0, &gCadet.transform.position);

            if (gInputMask[0] & InputMaskPlayer && getButtonDown(0, Z_TRIG) && (gLevelFlags & LEVEL_HAS_ROBOT)) {
                levelSwitchToRobot();
            }
        } else if (gRobot.controllerIndex != -1) {
            levelUpdateCamera(0, &gRobot.transform.position);

            if (gInputMask[0] & InputMaskPlayer && getButtonDown(0, Z_TRIG) && (gLevelFlags & LEVEL_HAS_CADET)) {
                levelSwitchToCadet();
            }
        }
    }

    if (gLevelFlags & LEVEL_INTRO_CUTSCENE) {
        if (gCurrentLevel == 0) {
            if (gRocket.animationTiming > 0.0f) {
                gScene.camera[0].targetPosition = gRocket.landingSpot;
            } else if (gCadet.actor.stateFlags & CADET_IS_INVISIBLE) {
                gCadet.actor.stateFlags &= ~CADET_IS_INVISIBLE;
                levelTitleEffectInit(&gLevelTitleEffect, gLoadedLevel->levelData->name);
                gScene.camera[0].targetPosition = gCadet.transform.position;
                gScene.camera[0].followDistanceStep = 0;
            }
        }

        if (!(gCadet.actor.stateFlags & CADET_IS_CUTSCENE)) {
            gLevelFlags &= ~LEVEL_INTRO_CUTSCENE;
            gInputMask[0] = INPUT_MASK_PLAY;
            gInputMask[1] = INPUT_MASK_PLAY;
            gScene.camera[0].followDistanceStep = 1;
            gScene.camera[1].followDistanceStep = 1;
        }
    }

    if (gLevelFlags & LEVEL_FOCUS_CUTSCENE) {
        gLevelCutsceneTimer -= gTimeDelta;

        if (gLevelCutsceneTimer < 0.0f) {
            gLevelFlags &= ~LEVEL_FOCUS_CUTSCENE;
            inputMaskPop();
            gScene.camera[gFocusCamera].followDistanceStep = 1;
        }
    }

    if (!(gLevelFlags & (LEVEL_INTRO_CUTSCENE | LEVEL_FOCUS_CUTSCENE)) && getButtonDown(0, START_BUTTON)) {
        audioRestartPlaySound(
            gPlayerSoundIds[SoundUISelect],
            0.5f,
            MENU_SELECT_SOUND_VOLUME,
            0.0f,
            10
        );
        pauseMenuShow(&gPauseMenu);
    }

    if ((!(gLevelFlags & LEVEL_HAS_CADET) || gCadetExit.isActive) && 
        (!(gLevelFlags & LEVEL_HAS_ROBOT) || gRobotExit.isActive || (gLevelFlags & LEVEL_IS_FINAL))) {
        cadetFinishLevel(&gCadet);
        if ((gLevelFlags & LEVEL_IS_FINAL)) {
            if (gCurrentPlayMode == LevelPlayModeCoOp) {
                renderSceneSetTargetViewportSplit(SCREEN_WD);
            }
        } else {
            robotFinishLevel(&gRobot);
        }
        saveFileMarkDidCompleteLevel(gCurrentLevel);
        gScene.camera[0].followDistanceStep = 0;
        gScene.camera[1].followDistanceStep = 0;
        gLevelFlags |= LEVEL_EXIT_CUTSCENE;
        gInputMask[0] = 0;
        gInputMask[1] = 0;

        int i;

        for (i = 0; i < gExpandedLevel.gemCount; ++i) {
            gemEndCutscene(&gExpandedLevel.gems[i], &gCadet.transform.position);
        }

        enum PlayerSounds exitSound = PlayerSoundsBothWarp;

        if (!(gLevelFlags & LEVEL_HAS_ROBOT) || (gLevelFlags & LEVEL_IS_FINAL)) {
            exitSound = PlayerSoundsCadetWarp;
        } else if (!(gLevelFlags & LEVEL_HAS_CADET)) {
            exitSound = PlayerSoundsRobotWarp;
        }

        audioPlaySound(
            gPlayerSoundIds[exitSound],
            0.5f,
            0.8f,
            0.0f,
            10
        );
    }

    if (gLevelFlags & LEVEL_EXIT_CUTSCENE) {
        if (gCadet.actor.stateFlags & CADET_IS_INVISIBLE) {
            if (gLevelFlags & LEVEL_IS_FINAL) {
                if (gRocket.rocketFlags & ROCKET_FLAGS_ANIMATION_DONE) {
                    saveFileSave();
                    if (saveFileCalculateGemsCollected(_level_group_all_levels_count) == _level_group_all_levels_count * 3) {
                        levelSetNext(SceneIndexGoodEndingCutscene, 1);
                    } else {
                        levelSetNext(SceneIndexBadEndingCutscene, 1);
                    }
                } else if (!(gRocket.rocketFlags & ROCKET_FLAGS_LAUNCHING)) {
                    rocketLaunch(&gRocket);
                    gScene.camera[0].targetPosition = gRocket.transform.position;
                    gScene.camera[0].followDistanceStep = 1;
                }
            } else {
                levelNext();
                gLevelFlags &= ~LEVEL_EXIT_CUTSCENE;
            }
        }
    }
    
    if (getButtonDown(0, R_JPAD) && getButton(0, L_TRIG) && saveFileIsLevelComplete(gCurrentLevel)) {
        levelNext();
    } else if (getButtonDown(0, L_JPAD) && getButton(0, L_TRIG)) {
        if (gCurrentLevel == 0) {
            levelSetNext(SceneIndexIntroCutscene, 1);
        } else {
            levelPrev();
        }
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
    // platforms

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

void levelLoad(struct LevelDefinition* levelDef, enum LevelPlayMode playMode) {
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
    playerSoundsUseBank(SoundBankLevel);

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

    if (gCurrentLevel == _level_group_all_levels_count-1) {
        gLevelFlags |= LEVEL_IS_FINAL;
    }

    gInputMask[0] = 0;
    gInputMask[1] = 0;

    gCurrentPlayMode = playMode;

    gLevelFlags |= LEVEL_HAS_CADET;

    cadetReset(&levelDef->levelData->cadetStart);
    entranceExitInit(&gCadetExit, &levelDef->levelData->cadetFinish, 1);

    if (levelDef->levelData->robotStart.z < 0.5 && levelDef->levelData->robotStart.x > -0.5) {
        if (saveFileCheckTutorial(SAVEFILE_LEARNED_FOUND_ROBOT)) {
            gLevelFlags |= LEVEL_HAS_ROBOT;
        } else {
            gLevelFlags |= LEVEL_INTRO_ROBOT;
        }

        robotReset(&levelDef->levelData->robotStart);

        if (gLevelFlags & LEVEL_IS_FINAL) {
            struct Vector3 rocketPos;
            vector3AddScaled(&levelDef->levelData->cadetFinish, &gRight, -2.0f, &rocketPos);
            rocketStartAt(&gRocket, &rocketPos);
        } else {
            entranceExitInit(&gRobotExit, &levelDef->levelData->robotFinish, 0);
        }
    }

    gCadet.controllerIndex = 0;

    if (gCurrentPlayMode == LevelPlayModeCoOp && gLevelFlags & (LEVEL_HAS_ROBOT | LEVEL_INTRO_ROBOT)) {
        gScene.activeViewportCount = 2;
        cameraInit(&gScene.camera[0], &levelDef->levelData->cadetStart, 0);
        cameraInit(&gScene.camera[1], &levelDef->levelData->robotStart, 1);

        gScene.viewports[0].minx = 0;
        gScene.viewports[0].maxx = SCREEN_WD/2-1;
        gScene.viewports[0].miny = 0;
        gScene.viewports[0].maxy = gScreenHeight;
        gScene.fov[0] = 70.0f;

        gScene.viewports[1].minx = SCREEN_WD/2+1;
        gScene.viewports[1].maxx = SCREEN_WD;
        gScene.viewports[1].miny = 0;
        gScene.viewports[1].maxy = gScreenHeight;
        gScene.fov[1] = 70.0f;

        if (gLevelFlags & LEVEL_INTRO_ROBOT) {
            renderSceneSetViewportSplit(SCREEN_WD);
            renderSceneSetTargetViewportSplit(SCREEN_WD);
        } else {
            gRobot.controllerIndex = 1;
        }
    } else {
        gScene.activeViewportCount = 1;
        cameraInit(&gScene.camera[0], &levelDef->levelData->cadetStart, 0);

        gScene.viewports[0].minx = 0;
        gScene.viewports[0].maxx = SCREEN_WD;
        gScene.viewports[0].miny = 0;
        gScene.viewports[0].maxy = gScreenHeight;
        gScene.fov[0] = 70.0f;
    }

    levelExpand(levelDef);
    timeAddListener(&gLevelUpdateListener, levelUpdate, 0, TimeUpdateGroupWorld);
    graphicsAddMenu(levelHudRender, 0, 1);

    gLoadedLevel = levelDef;

    gScene.transparentMaterials[TransparentMaterialTypeShadow] = _drop_shadow_material;
    gScene.transparentMaterials[TransparentMaterialTypeGem] = _gem_mat;
    gScene.transparentMaterialCleanup[TransparentMaterialTypeGem] = _gem_cleanup_mat;
    gScene.transparentMaterials[TransparentMaterialTypeShockwave] = _shockwave_mat;
    gScene.transparentMaterialCleanup[TransparentMaterialTypeShockwave] = _shockwave_mat_cleanup;
    gScene.transparentMaterials[TransparentMaterialTypeSmoke] = _smoke_mat;
    gTutorialMenu.state = 0;

    if (levelDef->levelData->musicRomStart && levelDef->levelData->musicRomEnd) {
        struct SeqPlayEvent seq;
        seq.loopStart = 0;
        seq.loopEnd = -1;
        seq.loopCount = -1;
        seq.playbackStart = 0;
        seq.romStart = levelDef->levelData->musicRomStart;
        seq.romEnd = levelDef->levelData->musicRomEnd;
        audioPlaySequence(&seq);
    }

    if (gCurrentLevel == 0) {
        struct Vector3 rocketPos;
        vector3AddScaled(&levelDef->levelData->cadetStart, &gRight, -2.0f, &rocketPos);
        gScene.camera[0].targetPosition = rocketPos;
        gScene.camera[0].centerPosition = rocketPos;
        cameraSetFollowDistance(&gScene.camera[0], 1);
        rocketLandAt(&gRocket, &rocketPos);
        gCadet.actor.stateFlags |= CADET_IS_INVISIBLE;
        gFadeTimer = FADE_DURATION;
    } else {
        levelTitleEffectInit(&gLevelTitleEffect, levelDef->levelData->name);
    }
}