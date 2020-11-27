
#include "cutscene.h"
#include "src/boot.h"
#include "src/graphics/renderscene.h"
#include "src/system/memory.h"
#include "src/level/level.h"
#include "slides.h"
#include "src/levelthemes/cutscene/theme.h"
#include <memory.h>

#define CUTSCENE_HEIGHT     180

struct CutscenePlayer gCutscenePlayer;
struct TimeUpdateListener gCutsceneListener;

void cutSceneUpdate(void* data) {
    gCutscenePlayer.currentTime += gTimeDelta;

    struct CutsceneFrame* currFrame = &gCutscenePlayer.cutscene->frames[gCutscenePlayer.currentFrame];

    if (gCutscenePlayer.currentTime >= currFrame->duration) {
        if (gCutscenePlayer.currentFrame + 1 >= gCutscenePlayer.cutscene->frameCount) {
            gNextLevel = gCutscenePlayer.targetLevel;
        } else {
            gCutscenePlayer.currentFrame++;
            gCutscenePlayer.currentTime = 0.0f;
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

    memset(state->cfb, 0, barHeight * SCREEN_WD * 2);

    romCopy(
        cutsceneBufferAtY(gAllSlideLocations[currFrame->slideIndex], yOffset),
        cutsceneBufferAtY(state->cfb, barHeight),
        2 * SCREEN_WD * CUTSCENE_HEIGHT
    );

    memset(cutsceneBufferAtY(state->cfb, barHeight + CUTSCENE_HEIGHT), 0, barHeight * SCREEN_WD * 2);
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

    renderSceneReset(&gScene);
    gScene.activeViewportCount = 0;

    gCutscenePlayer.cutscene = cutscene;
    gCutscenePlayer.currentFrame = 0;
    gCutscenePlayer.currentTime = 0.0f;
    gCutscenePlayer.targetLevel = nextLevel;

    graphicsAddMenu(cutSceneRender, 0, 1);

    timeAddListener(&gCutsceneListener, cutSceneUpdate, 0, TimeUpdateGroupWorld);
}