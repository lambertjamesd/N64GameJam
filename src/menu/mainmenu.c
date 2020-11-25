
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

struct TimeUpdateListener gMainMenuUpdate;

void mainMenuUpdate(void* data) {
    if (getButtonDown(0, START_BUTTON)) {
        gNextLevel = 0;
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

    gCadet.transform.position.x = 0.195248f;
    gCadet.transform.position.y = 0;
    gCadet.transform.position.z = -0.397314f;
    quatAxisAngle(&gUp, -19.744f * (M_PI / 180.0f), &gCadet.transform.rotation);
    gCadet.transform.scale = 1.0f;
    gCadet.controllerIndex = -1;
    gCadet.teleport.flags = 0;
    dynamicActorAddToGroup(&gScene.dynamicActors, &gCadet.transform, &gCadet, cadetRender, MATERIAL_INDEX_NOT_BATCHED);

    gRobot.transform.position.x = -1.48174f;
    gRobot.transform.position.y = 0.0f;
    gRobot.transform.position.z = -1.16179f;
    quatAxisAngle(&gUp, 24.1687f * (M_PI / 180.0f), &gRobot.transform.rotation);
    gRobot.transform.scale = 1.0f;
    gRobot.controllerIndex = -1;
    gRobot.teleport.flags = 0;
    dynamicActorAddToGroup(&gScene.dynamicActors, &gRobot.transform, &gRobot, robotRender, MATERIAL_INDEX_NOT_BATCHED);

    gRocket.transform.position.x = 1.18596f;
    gRocket.transform.position.y = 0.0f;
    gRocket.transform.position.z = -7.58793f;
    quatIdent(&gRocket.transform.rotation);
    gRocket.transform.scale = 1.0f;

    dynamicActorAddToGroup(&gScene.dynamicActors, &gRocket.transform, &gRocket, rocketRender, MATERIAL_INDEX_NOT_BATCHED);

    timeAddListener(&gMainMenuUpdate, mainMenuUpdate, 0, TimeUpdateGroupWorld);
    
}