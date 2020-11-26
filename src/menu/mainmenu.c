
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

struct TimeUpdateListener gMainMenuUpdate;
float gMainMenuTime;

#define MAIN_MENU_FADE_START_TIME       5.0f
#define MAIN_MENU_FADE_END_TIME         6.0f

#define BACKGROUND_ALPHA                128
#define MAX_LEVELS_PER_PAGE             6

struct Menu gMainMenu;
int gMainMenuSelectedLevel;


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
        "Cancel",
        MenuItemBack,
        .targetMenu = 0,
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
    menuGoBack(menu);
}

////////////////////

struct MenuItem gEraseConfirmMenuItems[] = {
    {
        "Erase",
        MenuItemAction,
        .action = eraseMenuConfirm,
        0,
    },
    {
        "Cancel",
        MenuItemBack,
        .targetMenu = 0,
        0
    }
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
};

////////////////////

struct MenuItem gMainMenuItems[] = {
    {
        "Continue",
        MenuItemAction,
        .action = mainMenuSelectLevel,
        0,
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
    "Telelocation: Gemini",
    gMainMenuItems,
    3,
};

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
}

void mainMenuUpdate(void* data) {
    if (gMainMenuTime < MAIN_MENU_FADE_END_TIME && gMainMenuTime > MAIN_MENU_FADE_START_TIME) {
        float finalAlpha = mathfLerp(255.0f, BACKGROUND_ALPHA, (gMainMenuTime - MAIN_MENU_FADE_START_TIME) / (MAIN_MENU_FADE_END_TIME - MAIN_MENU_FADE_START_TIME));
        gRocket.color.r = gRocket.color.g = gRocket.color.b = (u8)finalAlpha;
    } else if (gMainMenuTime >= MAIN_MENU_FADE_END_TIME) {
        gRocket.color.r = BACKGROUND_ALPHA;
        gRocket.color.g = BACKGROUND_ALPHA;
        gRocket.color.b = BACKGROUND_ALPHA;

        menuUpdate(&gMainMenu);
        menuHandleInput(&gMainMenu, 0);

        if (gMainMenu.exitAnimationLevel == -1 && gMainMenuSelectedLevel != -1) {
            gNextLevel = gMainMenuSelectedLevel;
        }
    }

    gMainMenuTime += gTimeDelta;

    struct Vector3 eulerAngles;
    struct Vector3 position;

    mainMenuRocketPosition(gMainMenuTime, &position, &eulerAngles);
    quatEulerAngles(&eulerAngles, &gRocket.transform.rotation);

    vector3Lerp(&gRocket.transform.position, &position, 0.01f, & gRocket.transform.position);

    if (getButtonDown(0, START_BUTTON)) {
        gNextLevel = 0;
    }
}

void mainMenuBuildLevelSelect() {
    struct MenuItem* lastItem = 0;

    int itemIndex;

    for (itemIndex = 0; itemIndex < _level_group_all_levels_count; itemIndex += MAX_LEVELS_PER_PAGE) {
        int count = 0;
        int hasNext = 0;

        if (itemIndex + MAX_LEVELS_PER_PAGE < _level_group_all_levels_count) {
            hasNext = 1;
            count = MAX_LEVELS_PER_PAGE;
        } else {
            count = _level_group_all_levels_count - itemIndex;
        }

        struct MenuItem* items = heapMalloc(
            (count+1+hasNext) * sizeof(struct MenuItem), 
            ALIGNMENT_OF(struct MenuItem)
        );

        items[0].text = "<";
        items[0].targetMenu = 0;
        items[0].type = MenuItemBack;
        items[0].data = 0;

        int i;
        for (i = 1; i <= count; ++i) {
            items[i].text = _level_group_all_levels[itemIndex+i-1].name;
            items[i].action = mainMenuSelectLevel;
            items[i].data = (void*)(itemIndex+i-1);
            items[i].type = MenuItemAction;
        }

        if (itemIndex == 0) {
            gLevelSelectGroup.items = items;
            gLevelSelectGroup.itemCount = count+1+hasNext;
        } else {
            struct MenuItemGroup* selectGroup = heapMalloc(
                sizeof(struct MenuItemGroup),
                ALIGNMENT_OF(struct MenuItemGroup)
            );

            if (lastItem) {
                lastItem->text = ">";
                lastItem->targetMenu = selectGroup;
                lastItem->type = MenuItemMenu;
                lastItem->data = 0;
            }

            selectGroup->title = selectGroup->title;
            selectGroup->items = items;
            selectGroup->itemCount = count+1+hasNext;
        }

        if (hasNext) {
            lastItem = &items[count+1];
        } else {
            lastItem = 0;
        }
    }
}

void mainMenuInit() {
    heapReset();
    osUnmapTLBAll();

    u32 len = (u32)(_staticSegmentRomEnd - _staticSegmentRomStart);
    char* staticSegment = heapMalloc(len, 8);
    romCopy(_staticSegmentRomStart, (char*)staticSegment, len);

    // len = (u32)(_spinning_logoSegmentRomEnd - _spinning_logoSegmentRomStart);
    // u32 levelAlign;
    // u32 levelPageMask;
    // tlbAlign(len, &levelAlign, &levelPageMask);

    // char* levelSegment = heapMalloc(len, levelAlign);
    // romCopy(_spinning_logoSegmentRomStart, levelSegment, len);
    // osMapTLB(0, levelPageMask, (void*)(LEVEL_SEGMENT << 24), osVirtualToPhysical(levelSegment), -1, -1);

    timeResetListeners();
    graphicsClearMenus();

    graphicsInitLevel(
        staticSegment, 
        0, 
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
    gScene.viewports[0].maxy = SCREEN_HT;
    gScene.fov[0] = 54.188044907f;

    gMainMenuTime = 0.0f;

    struct Vector3 eulerAngles;
    mainMenuRocketPosition(gMainMenuTime, &gRocket.transform.position, &eulerAngles);
    gRocket.transform.position.x -= 10.0f;
    quatEulerAngles(&eulerAngles, &gRocket.transform.rotation);
    gRocket.transform.scale = 1.0f;

    gRocket.color.r = 255;
    gRocket.color.g = 255;
    gRocket.color.b = 255;
    gRocket.color.a = 255;
    
    gMainMenuSelectedLevel = -1;

    dynamicActorAddToGroup(&gScene.dynamicActors, &gRocket.transform, &gRocket, rocketRender, MATERIAL_INDEX_NOT_BATCHED);

    timeAddListener(&gMainMenuUpdate, mainMenuUpdate, 0, TimeUpdateGroupWorld);

    mainMenuBuildLevelSelect();
    graphicsAddMenu(menuRender, &gMainMenu, 1);
    menuInit(&gMainMenu, &gMainMenuGroup);
}