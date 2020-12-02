
#include "cutscene.h"
#include "src/boot.h"
#include "src/graphics/renderscene.h"
#include "src/system/memory.h"
#include "src/level/level.h"
#include "src/audio/audio.h"
#include "slides.h"
#include <memory.h>

#define CUTSCENE_HEIGHT     180
#define MAX_CUTSCENE_HEIGHT 480

struct CutscenePlayer gCutscenePlayer;
struct TimeUpdateListener gCutsceneListener;
char* gCutsceneBuffer;
int gCurrentMaxY = 0;

void cutSceneUpdate(void* data) {
    gCutscenePlayer.currentTime += gTimeDelta;
    gCutscenePlayer.totalTime += gTimeDelta;

    if (gCutscenePlayer.nextEvent < gCutscenePlayer.cutscene->eventCount) {
        struct CutsceneEvent* currentEvent = &gCutscenePlayer.cutscene->events[gCutscenePlayer.nextEvent];
        if (gCutscenePlayer.totalTime >= currentEvent->at) {
            switch (currentEvent->eventType) {
                case CutsceneEventTypeSeq:
                    audioPlaySequence(&currentEvent->seq);
                    break;
                case CutsceneEventTypeSeqStop:
                    audioStopSequence();
                    break;
                case CutsceneEventTypeSound:
                    audioPlaySound(
                        gPlayerSoundIds[currentEvent->sound.soundId],
                        currentEvent->sound.pitch,
                        currentEvent->sound.priority,
                        currentEvent->sound.pan,
                        currentEvent->sound.priority
                    );
                    break;
                case CutsceneEventTypeSoundStop:
                    audioStopSound(gPlayerSoundIds[currentEvent->sound.soundId]);
                    break;
            }

            gCutscenePlayer.nextEvent++;
        }
    }

    struct CutsceneFrame* currFrame = &gCutscenePlayer.cutscene->frames[gCutscenePlayer.currentFrame];

    if (gCutscenePlayer.currentTime >= currFrame->duration) {
        if (gCutscenePlayer.currentFrame + 1 >= gCutscenePlayer.cutscene->frameCount) {
            gNextLevel = gCutscenePlayer.targetLevel;
            audioStopSequence();
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

#define MAX_TILE_X  64
#define MAX_TILE_Y  32

void graphicsCopyImage(struct GraphicsState* state, char* source, int iw, int ih, int sx, int sy, int dx, int dy, int w, int h) {
    gDPPipeSync(state->dl++);
    gDPSetCycleType(state->dl++, G_CYC_1CYCLE);
    gDPSetRenderMode(state->dl++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
    gDPSetCombineMode(state->dl++, G_CC_DECALRGB, G_CC_DECALRGB);
    gDPSetTextureLUT(state->dl++, G_TT_NONE);
    gDPSetTexturePersp(state->dl++, G_TP_NONE);
    gDPSetPrimColor(state->dl++, 0, 0, 255, 0, 255, 255);

    int tileXCount = (w + MAX_TILE_X-1) / MAX_TILE_X;
    int tileYCount = (h + MAX_TILE_Y-1) / MAX_TILE_Y;
    int tileX, tileY;

    for (tileX = 0; tileX < tileXCount; ++tileX) {
        int currX = tileX * MAX_TILE_X;
        int tileWidth = w - currX;

        if (tileWidth > MAX_TILE_X) {
            tileWidth = MAX_TILE_X;
        }

        for (tileY = 0; tileY < tileYCount; ++tileY) {
            int currY = tileY * MAX_TILE_Y;
            int tileHeight = h - currY;
            
            if (tileHeight > MAX_TILE_Y) {
                tileHeight = MAX_TILE_Y;
            }
            
            gDPLoadTextureTile(
                state->dl++,
                K0_TO_PHYS(source + ((sx + currX) + (sy + currY) * iw) * 2),
                G_IM_FMT_RGBA, G_IM_SIZ_16b,
                iw, ih,
                0, 0,
                tileWidth-1, tileHeight-1,
                0,
                G_TX_CLAMP, G_TX_CLAMP,
                G_TX_NOMASK, G_TX_NOMASK,
                G_TX_NOLOD, G_TX_NOLOD
            );
            
            gSPTextureRectangle(
                state->dl++,
                (dx+currX) << 2, (dy+currY) << 2,
                (dx+currX+tileWidth) << 2, (dy+currY+tileHeight) << 2,
                G_TX_RENDERTILE, 
                0, 0, 1 << 10, 1 << 10
            );
        }
    }

    gDPPipeSync(state->dl++);
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
    
    if (gCurrentMaxY < yOffset + CUTSCENE_HEIGHT) {
        romCopy(
            cutsceneBufferAtY(gAllSlideLocations[currFrame->slideIndex], gCurrentMaxY),
            cutsceneBufferAtY(gCutsceneBuffer, gCurrentMaxY),
            2 * SCREEN_WD * (yOffset + CUTSCENE_HEIGHT - gCurrentMaxY)
        );
        gCurrentMaxY = yOffset + CUTSCENE_HEIGHT;
        osWritebackDCacheAll();
    }

    graphicsCopyImage(state, gCutsceneBuffer, 
        SCREEN_WD, CUTSCENE_HEIGHT, 
        0, yOffset, 
        0, barHeight, 
        SCREEN_WD, CUTSCENE_HEIGHT
    );

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
        0
    );

    gCutsceneBuffer = heapMalloc(SCREEN_WD*MAX_CUTSCENE_HEIGHT*2,16);

    renderSceneReset(&gScene);
    gScene.activeViewportCount = 0;

    gCutscenePlayer.cutscene = cutscene;
    gCutscenePlayer.currentFrame = 0;
    gCutscenePlayer.currentTime = 0.0f;
    gCutscenePlayer.totalTime = 0.0f;
    gCutscenePlayer.targetLevel = nextLevel;
    gCutscenePlayer.nextEvent = 0;
    gCurrentMaxY = 0;

    graphicsAddMenu(cutSceneRender, 0, 1);

    if (cutscene->soundBank != SoundBankNone) {
        playerSoundsUseBank(cutscene->soundBank);
    }

    timeAddListener(&gCutsceneListener, cutSceneUpdate, 0, TimeUpdateGroupWorld);
}