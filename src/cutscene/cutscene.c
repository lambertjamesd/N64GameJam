
#include "cutscene.h"
#include "src/boot.h"
#include "src/graphics/renderscene.h"
#include "src/system/memory.h"
#include "src/level/level.h"
#include "slides.h"
#include "src/levelthemes/cutscene/theme.h"
#include <memory.h>

#define CUTSCENE_HEIGHT     180
#define MAX_CUTSCENE_HEIGHT 480

struct CutscenePlayer gCutscenePlayer;
struct TimeUpdateListener gCutsceneListener;
char* gCutsceneBuffer;
int gCurrentMaxY = 0;
int gNeedsClearBars = 0;

void cutSceneUpdate(void* data) {
    gCutscenePlayer.currentTime += gTimeDelta;

    struct CutsceneFrame* currFrame = &gCutscenePlayer.cutscene->frames[gCutscenePlayer.currentFrame];

    if (gCutscenePlayer.currentTime >= currFrame->duration) {
        if (gCutscenePlayer.currentFrame + 1 >= gCutscenePlayer.cutscene->frameCount) {
            gNextLevel = gCutscenePlayer.targetLevel;
        } else {
            gCutscenePlayer.currentFrame++;
            gCutscenePlayer.currentTime = 0.0f;
            gCurrentMaxY = 0;
        }
    }
}

char* cutsceneBufferAtY(char* input, int y) {
    return input + y * SCREEN_WD * 2;
}

void cutSceneRender(void* data, struct GraphicsState* state, struct FontRenderer* fontRenderer) {
    if (gCutscenePlayer.currentFrame >= gCutscenePlayer.cutscene->frameCount) {
        return;
    }

    struct CutsceneFrame* currFrame = &gCutscenePlayer.cutscene->frames[gCutscenePlayer.currentFrame];


    if (currFrame->slideIndex >= gAllSlideCount || currFrame->slideIndex < 0) {
        return;
    }

    int yOffset = (int)(gCutscenePlayer.currentTime * currFrame->yVelocity + currFrame->yOffset);

    if (yOffset < 0) {
        yOffset = 0;
    }
    
    int barHeight = (SCREEN_HT - CUTSCENE_HEIGHT) >> 1;

    if (gNeedsClearBars) {
        memset(state->cfb, 0, barHeight * SCREEN_WD * 2);
        memset(cutsceneBufferAtY(state->cfb, barHeight + CUTSCENE_HEIGHT), 0, barHeight * SCREEN_WD * 2);
        gNeedsClearBars--;
    }
    
    if (gCurrentMaxY < yOffset + CUTSCENE_HEIGHT) {
        romCopy(
            cutsceneBufferAtY(gAllSlideLocations[currFrame->slideIndex], gCurrentMaxY),
            cutsceneBufferAtY(gCutsceneBuffer, gCurrentMaxY),
            2 * SCREEN_WD * (yOffset + CUTSCENE_HEIGHT - gCurrentMaxY)
        );
        gCurrentMaxY = yOffset + CUTSCENE_HEIGHT;
    }

    memcpy(
        cutsceneBufferAtY(state->cfb, barHeight),
        cutsceneBufferAtY(gCutsceneBuffer, yOffset),
        2 * SCREEN_WD * CUTSCENE_HEIGHT
    );
    osWritebackDCacheAll();
}

void cutScenePlay(struct Cutscene* cutscene, int nextLevel) {
    heapReset();
    osUnmapTLBAll();

    u32 len = (u32)(_staticSegmentRomEnd - _staticSegmentRomStart);
    char* staticSegment = heapMalloc(len, 8);
    romCopy(_staticSegmentRomStart, (char*)staticSegment, len);
    
    timeResetListeners();
    graphicsClearMenus();

    graphicsInitLevel(
        staticSegment, 
        0, 
        0, 
        0,
        &gCutsceneTheme
    );

    gCutsceneBuffer = heapMalloc(SCREEN_WD*MAX_CUTSCENE_HEIGHT*2,16);

    renderSceneReset(&gScene);
    gScene.activeViewportCount = 0;

    gCutscenePlayer.cutscene = cutscene;
    gCutscenePlayer.currentFrame = 0;
    gCutscenePlayer.currentTime = 0.0f;
    gCutscenePlayer.targetLevel = nextLevel;
    gCurrentMaxY = 0;
    gNeedsClearBars = 2;

    graphicsAddMenu(cutSceneRender, 0, 1);

    timeAddListener(&gCutsceneListener, cutSceneUpdate, 0, TimeUpdateGroupWorld);
}