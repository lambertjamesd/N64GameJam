
#include "mainmenu.h"

#include <ultra64.h>

#include "src/system/memory.h"
#include "src/boot.h"
#include "src/cadet/cadet.h"
#include "src/robot/robot.h"
#include "src/font/font.h"
#include "src/rocket/rocket.h"
#include "src/input/controller.h"
#include "src/level/level.h"
#include "src/math/mathf.h"
#include "src/font/endlessbossbattle/endlessbossbattle.h"
#include "src/menu/menu.h"
#include "src/levels/levels.h"
#include "src/save/savefile.h"
#include "src/audio/playersounds.h"
#include "src/font/buttons/buttons.h"
#include "src/audio/audio.h"
#include "src/audio/allseq.h"

extern char _titlescreenSegmentRomStart[];
extern char _titlescreenSegmentRomEnd[];
extern unsigned short _titleScreenRendered_tex[];

struct TimeUpdateListener gMainMenuUpdate;
float gMainMenuTime;
float gMainMenuExitTime;

#define MAIN_MENU_FADE_START_TIME       5.0f
#define MAIN_MENU_FADE_END_TIME         6.0f
#define MAIN_MENU_EXIT_DURATION         3.0f
#define MAIN_MENU_FADE_IN_TIME          0.5f

#define BACKGROUND_ALPHA                64
#define MAX_LEVELS_PER_PAGE             6

struct Menu gMainMenu;
int gMainMenuSelectedLevel;
int gMainMenuUnlockedLevels;
int gMainMenuTotalGems;

int mainMenuBuildLevelSelect();

short gKonamiCodeButtons[] = {
    U_JPAD,
    U_JPAD,
    D_JPAD,
    D_JPAD,
    L_JPAD,
    R_JPAD,
    L_JPAD,
    R_JPAD,
    B_BUTTON,
    A_BUTTON,
};
short gKonamiCodeIndex = 0;

struct MenuItemGroup gNewGameGroup;


////////////////////

void mainMenuStart(struct Menu* menu, void* data) {
    gMainMenu.itemStackDepth = -1;
    gMainMenu.insertAnimationLevel = -1;
    
    if (data) {
        gCurrentPlayMode = LevelPlayModeCoOp;
    } else {
        gCurrentPlayMode = LevelPlayModeSingle;
    }
}


////////////////////

struct MenuItem gSelectCoopItems[] = {
    {
        "Single Player",
        MenuItemAction,
        .action = mainMenuStart,
        0,
    },
    {
        "Co-op",
        MenuItemAction,
        .action = mainMenuStart,
        (void*)1,
    },
    {
        "Back",
        MenuItemBack,
        .popDistance = 1,
        0
    }
};

struct MenuItemGroup gSelectCoopGroup = {
    "Players",
    gSelectCoopItems,
    3,
};

////////////////////

void mainMenuSelectLevel(struct Menu* menu, void* data) {
    gMainMenuSelectedLevel = (int)data;
    menuPush(menu, &gSelectCoopGroup);

}

void eraseMenuConfirm(struct Menu* menu, void* data) {
    saveFileErase();
    gMainMenu.current[0] = &gNewGameGroup;
    gMainMenu.selected[0] = 0;
    menuGoBack(menu);
}

////////////////////

struct MenuItem gEraseConfirmMenuItems[] = {
    {
        "Cancel",
        MenuItemBack,
        .popDistance = 1,
        0
    },
    {
        "Erase",
        MenuItemAction,
        .action = eraseMenuConfirm,
        0,
    },
};

struct MenuItemGroup gEraseMenuItemGroup = {
    "Are you sure?",
    gEraseConfirmMenuItems,
    2,
};

////////////////////

struct MenuItemGroup gLevelSelectGroup = {
    "Level Select",
    0,
    6,
    MenuTypeList,
};

////////////////////


struct MenuItem gNewGameItems[] = {
    {
        "New Game",
        MenuItemAction,
        .action = mainMenuSelectLevel,
        (void*)SceneIndexIntroCutscene,
    },
};

struct MenuItemGroup gNewGameGroup = {
    "",
    gNewGameItems,
    1,
};

struct MenuItem gMainMenuItems[] = {
    {
        "Continue",
        MenuItemAction,
        .action = mainMenuSelectLevel,
        (void*)-1,
    },
    {
        "Level Select",
        MenuItemMenu,
        .targetMenu = &gLevelSelectGroup,
        0,
    },
    {
        "Erase",
        MenuItemMenu,
        .targetMenu = &gEraseMenuItemGroup,
        0,
    },
};

struct MenuItemGroup gMainMenuGroup = {
    "",
    gMainMenuItems,
    3,
    MenuTypeLarge,
    0,
    28,
};

void levelSelectPrepGems(struct MenuItemGroup* group, struct GraphicsState* state, struct FontRenderer* fontRenderer) {
    gDPPipeSync(state->dl++);
    gSPDisplayList(state->dl++, gButtonFontUse);
}

void levelSelectRenderGems(void* data, int x, int y, int selected, struct GraphicsState* state, struct FontRenderer* fontRenderer) {
    char gemString[4];
    int levelIndex = (int)data;
    int i;
    for (i = 0; i < 3; ++i) {
        if (saveFileDidCollectGem(levelIndex, i)) {
            gemString[i] = ButtonFontMappingGem;
        } else {
            gemString[i] = ButtonFontMappingGemGone;
        }
    }
    gemString[3] = 0;

    fontRendererDrawCharacters(
        fontRenderer,
        &gButtonFont,
        &state->dl,
        gemString,
        x - 39,
        y - 2
    );
}

void mainMenuRocketPosition(float time, struct Vector3* out, struct Vector3* eulerAngles) {
    out->x = -2.98266f;
    out->y = 1.18541f;
    out->z = -5.48465f;

    float s0 = sinf(time * (4.0f / 5.0f)) * 0.1f;
    float s1 = sinf(time * (4.0f / 7.0f)) * 0.1f;
    float s2 = sinf(time * (4.0f / 13.0f)) * 0.2f;
    float s3 = sinf(time * (4.0f / 17.0f)) * 0.3f;
    float s4 = sinf(time * (4.0f / 19.0f)) * 0.3f;

    out->x += s0 + s2;
    out->y += s1 + s3;
    out->z += s2 + s4;

    eulerAngles->x = (s1 + s2) * 0.2f;
    eulerAngles->y = M_PI * 0.5f + (s1 + s4);
    eulerAngles->z = -M_PI * 0.5f + (s0 + s3) * 0.2f;

    out->x += 4.0f * gMainMenuExitTime * gMainMenuExitTime;
}

#define GEM_COUNT_X     280
#define GEM_COUNT_Y     200

#define TITLE_SCREEN_WIDTH          256
#define TITLE_SCREEN_TILE_HEIGHT    8
#define TITLE_SCREEN_TILES          8
#define TITLE_SCREEN_LOCATION       30

void mainMenuRenderImage(struct GraphicsState* state, int y) {
    gDPPipeSync(state->dl++);

    gDPSetCycleType(state->dl++, G_CYC_1CYCLE);
    gDPSetRenderMode(state->dl++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    gDPSetCombineMode(state->dl++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    gDPSetTextureLUT(state->dl++, G_TT_NONE);
    gDPSetTexturePersp(state->dl++, G_TP_NONE);
    gDPSetPrimColor(state->dl++, 0, 0, 255, 255, 255, 255);

    int i;
    for (i = 0; i < TITLE_SCREEN_TILES; ++i) {
        int tileHeight = SCALE_FOR_PAL(TITLE_SCREEN_TILE_HEIGHT);

        if (y + tileHeight > 0) {
            int yStart = y;
            int ySrcy = i * TITLE_SCREEN_TILE_HEIGHT;

            if (yStart < 0) {
                ySrcy -= UNSCALE_FOR_PAL(yStart);
                tileHeight += yStart;
                yStart = 0;
            }

            gDPLoadTextureTile(
                state->dl++,
                _titleScreenRendered_tex + ySrcy * TITLE_SCREEN_WIDTH,
                G_IM_FMT_RGBA, G_IM_SIZ_16b,
                256, 8,
                0, 0,
                255, 7,
                0,
                G_TX_CLAMP, G_TX_CLAMP,
                G_TX_NOMASK, G_TX_NOMASK,
                G_TX_NOLOD, G_TX_NOLOD
            );

            gSPTextureRectangle(
                state->dl++,
                32 << 2, (yStart) << 2,
                288 << 2, (yStart + tileHeight) << 2,
                G_TX_RENDERTILE, 
                0, 0, 1 << 10, (TITLE_SCREEN_TILE_HEIGHT << 10) / SCALE_FOR_PAL(TITLE_SCREEN_TILE_HEIGHT)
            );
        }


        y += tileHeight;
    }

    gDPPipeSync(state->dl++);
}

void mainMenuRender(void* data, struct GraphicsState* state, struct FontRenderer* fontRenderer) {
    menuRender(data, state, fontRenderer);

    if (gMainMenuTotalGems && gMainMenu.itemStackDepth != 0) {
        gSPDisplayList(state->dl++, gEndlessBossBattleUse);
        char buffer[16];
        sprintf(buffer, "%d/%d", gMainMenuTotalGems, 3 * _level_group_all_levels_count);
        fontRendererSetScale(fontRenderer, 1.0f, 1.0f);

        float w = fontRendererMeasureWidth(&gEndlessBossBattle, buffer);
        
        fontRendererDrawCharacters(
            fontRenderer,
            &gEndlessBossBattle,
            &state->dl,
            buffer,
            GEM_COUNT_X - (int)w,
            GEM_COUNT_Y
        );

        gDPPipeSync(state->dl++);
        gSPDisplayList(state->dl++, gButtonFontUse);

        buffer[0] = ButtonFontMappingGem;
        buffer[1] = '\0';

        fontRendererDrawCharacters(
            fontRenderer,
            &gButtonFont,
            &state->dl,
            buffer,
            GEM_COUNT_X + 3,
            GEM_COUNT_Y - 2
        );
        gDPPipeSync(state->dl++);
    }

    if (gMainMenu.itemStackDepth == 0 || gMainMenu.exitAnimationLevel == 0) {
        if (gMainMenu.exitAnimationLevel != 0) {
            float animLerp = 1.0f - menuGetAnimationLerp(&gMainMenu);
            mainMenuRenderImage(state, SCALE_FOR_PAL(
                    TITLE_SCREEN_LOCATION - 
                    animLerp * animLerp * (TITLE_SCREEN_TILE_HEIGHT * TITLE_SCREEN_TILES + TITLE_SCREEN_LOCATION)
            )); 
        } else if (gMainMenu.insertAnimationLevel != 0) {
            float animLerp = menuGetAnimationLerp(&gMainMenu);
            mainMenuRenderImage(state, SCALE_FOR_PAL(
                    TITLE_SCREEN_LOCATION - 
                    animLerp * animLerp * (TITLE_SCREEN_TILE_HEIGHT * TITLE_SCREEN_TILES + TITLE_SCREEN_LOCATION)
            )); 
        } else {
            mainMenuRenderImage(state, SCALE_FOR_PAL(TITLE_SCREEN_LOCATION));    
        }
    }
}

void mainMenuUpdate(void* data) {
    if (getButtonDown(0, gKonamiCodeButtons[gKonamiCodeIndex])) {
        ++gKonamiCodeIndex;

        if (gKonamiCodeIndex == sizeof(gKonamiCodeButtons)/sizeof(*gKonamiCodeButtons)) {
            saveUnlockAll();
            gMainMenuUnlockedLevels = mainMenuBuildLevelSelect();
            gMainMenu.current[0] = &gMainMenuGroup;
            gKonamiCodeIndex = 0;
            gMainMenuTotalGems = _level_group_all_levels_count * 3;

            audioPlaySound(
                gPlayerSoundIds[GoalTouchSmall],
                0.5f,
                1.0f,
                0.0f,
                10
            );
        }
    } else if (getButtonDown(0, ~gKonamiCodeButtons[gKonamiCodeIndex])) {
        gKonamiCodeIndex = 0;
    }

    if (gMainMenuTime < MAIN_MENU_FADE_END_TIME && gMainMenuTime > MAIN_MENU_FADE_START_TIME) {
        float finalAlpha = mathfLerp(255.0f, BACKGROUND_ALPHA, (gMainMenuTime - MAIN_MENU_FADE_START_TIME) / (MAIN_MENU_FADE_END_TIME - MAIN_MENU_FADE_START_TIME));
        gRocket.color.r = gRocket.color.g = gRocket.color.b = (u8)finalAlpha;
        gRocket.trail.alpha = (u8)finalAlpha;
    } else if (gMainMenuTime >= MAIN_MENU_FADE_END_TIME) {
        if (gMainMenu.exitAnimationLevel == -1 && gMainMenuSelectedLevel != -2) {
            if (gMainMenuExitTime < MAIN_MENU_FADE_IN_TIME) {
                float finalAlpha = mathfLerp(BACKGROUND_ALPHA, 255.0f, gMainMenuExitTime / MAIN_MENU_FADE_IN_TIME);
                gRocket.color.r = gRocket.color.g = gRocket.color.b = (u8)finalAlpha;
                gRocket.trail.alpha = (u8)finalAlpha;
            } else if (gMainMenuExitTime < MAIN_MENU_EXIT_DURATION) {
                float finalAlpha = mathfLerp(0.0f, 255.0f, (MAIN_MENU_EXIT_DURATION - gMainMenuExitTime) / 0.5f);

                if (finalAlpha > 255.0f) {
                    gRocket.color.r = gRocket.color.g = gRocket.color.b = 255;
                    gRocket.trail.alpha = 255;
                } else {
                    gRocket.color.r = gRocket.color.g = gRocket.color.b = (u8)finalAlpha;
                    gRocket.trail.alpha = (u8)finalAlpha;
                }

                audioStopSequence(MAIN_MENU_EXIT_DURATION - MAIN_MENU_FADE_IN_TIME);
            } else {
                if (gMainMenuSelectedLevel == -1) {
                    gNextLevel = gMainMenuUnlockedLevels - 1;
                } else {
                    gNextLevel = gMainMenuSelectedLevel;
                }

                audioStopSequence(0.0f);
            }

            gMainMenuExitTime += gTimeDelta;
        } else {
            gRocket.color.r = BACKGROUND_ALPHA;
            gRocket.color.g = BACKGROUND_ALPHA;
            gRocket.color.b = BACKGROUND_ALPHA;
            gRocket.trail.alpha = BACKGROUND_ALPHA;

            menuUpdate(&gMainMenu);
            menuHandleInput(&gMainMenu, 0);
        }
    }

    gMainMenuTime += gTimeDelta;

    struct Vector3 eulerAngles;
    struct Vector3 position;

    mainMenuRocketPosition(gMainMenuTime, &position, &eulerAngles);
    quatEulerAngles(&eulerAngles, &gRocket.transform.rotation);

    vector3Lerp(&gRocket.transform.position, &position, 0.01f, & gRocket.transform.position);
}

int mainMenuBuildLevelSelect() {
    struct MenuItem* lastItem = 0;

    int completeLevels = 0;

    while (saveFileIsLevelComplete(completeLevels) && completeLevels < _level_group_all_levels_count) {
        ++completeLevels;
    }

    if (completeLevels < _level_group_all_levels_count) {
        completeLevels++;
    }

    struct MenuItem* items = heapMalloc(
        (completeLevels+1) * sizeof(struct MenuItem), 
        ALIGNMENT_OF(struct MenuItem)
    );

    items[0].text = "Back";
    items[0].targetMenu = 0;
    items[0].type = MenuItemBack;
    items[0].data = 0;
    items[0].renderMore = 0;

    int i;
    for (i = 1; i <= completeLevels; ++i) {
        items[i].text = _level_group_all_levels[i-1].name;
        items[i].action = mainMenuSelectLevel;
        items[i].data = (void*)(i-1);
        items[i].type = MenuItemAction;
        items[i].renderMore = levelSelectRenderGems;
    }

    gLevelSelectGroup.items = items;
    gLevelSelectGroup.itemCount = completeLevels+1;
    gLevelSelectGroup.renderMore = levelSelectPrepGems;

    return completeLevels;
}

void mainMenuInit() {
    heapReset();
    osUnmapTLBAll();

    u32 len = (u32)(_staticSegmentRomEnd - _staticSegmentRomStart);
    char* staticSegment = heapMalloc(len, 8);
    romCopy(_staticSegmentRomStart, (char*)staticSegment, len);

    len = (u32)(_titlescreenSegmentRomEnd - _titlescreenSegmentRomStart);
    u32 levelAlign;
    u32 levelPageMask;
    tlbAlign(len, &levelAlign, &levelPageMask);

    char* levelSegment = heapMalloc(len, levelAlign);
    romCopy(_titlescreenSegmentRomStart, (char*)levelSegment, len);
    osMapTLB(0, levelPageMask, (void*)(LEVEL_SEGMENT << 24), osVirtualToPhysical(levelSegment), -1, -1);

    timeResetListeners();
    graphicsClearMenus();

    graphicsInitLevel(
        staticSegment, 
        levelSegment, 
        0, 
        0,
        0
    );

    renderSceneReset(&gScene);

    gScene.activeViewportCount = 1;

    gScene.camera[0].transform.position.x = -0.690682f;
    gScene.camera[0].transform.position.y = 1.21354f;
    gScene.camera[0].transform.position.z = 2.62733f;

    quatAxisAngle(&gRight, 0.0f, &gScene.camera[0].transform.rotation);

    gScene.camera[0].transform.scale = 1.0f;

    gScene.viewports[0].minx = 0;
    gScene.viewports[0].maxx = SCREEN_WD;
    gScene.viewports[0].miny = 0;
    gScene.viewports[0].maxy = gScreenHeight;
    gScene.fov[0] = 54.188044907f;
    gScene.transparentMaterials[TransparentMaterialTypeShockwave] = _shockwave_mat;
    gScene.transparentMaterialCleanup[TransparentMaterialTypeShockwave] = _shockwave_mat_cleanup;
    

    gMainMenuTime = 0.0f;
    gMainMenuExitTime = 0.0f;

    struct Vector3 eulerAngles;
    mainMenuRocketPosition(gMainMenuTime, &gRocket.transform.position, &eulerAngles);
    gRocket.transform.position.x -= 10.0f;
    quatEulerAngles(&eulerAngles, &gRocket.transform.rotation);
    gRocket.transform.scale = 1.0f;

    gRocket.color.r = 255;
    gRocket.color.g = 255;
    gRocket.color.b = 255;
    gRocket.color.a = 255;

    rocketTrailStart(&gRocket.trail, &gRocket.transform, &gRocketTrailParameters, ROCKET_TRAIL_PARAMETER_COUNT, 0);
    
    gMainMenuSelectedLevel = -2;

    dynamicActorAddToGroup(&gScene.dynamicActors, &gRocket.transform, &gRocket, rocketRender, MATERIAL_INDEX_NOT_BATCHED, 6.3f);

    timeAddListener(&gMainMenuUpdate, mainMenuUpdate, 0, TimeUpdateGroupWorld);
    playerSoundsUseBank(SoundBankLevel);

    struct SeqPlayEvent seq;
    seq.loopEnd = -1;
    seq.loopStart = 0;
    seq.loopCount = -1;
    seq.playbackStart = 0;
    seq.romStart = _thermalImagingSegmentRomStart;
    seq.romEnd =_thermalImagingSegmentRomEnd;
    audioPlaySequence(&seq);

    gMainMenuTotalGems = saveFileCalculateGemsCollected(_level_group_all_levels_count);
    gMainMenuUnlockedLevels = mainMenuBuildLevelSelect();
    graphicsAddMenu(mainMenuRender, &gMainMenu, 1);
    if (gMainMenuUnlockedLevels > 1) {
        menuInit(&gMainMenu, &gMainMenuGroup);
    } else {
        menuInit(&gMainMenu, &gNewGameGroup);
    }
}