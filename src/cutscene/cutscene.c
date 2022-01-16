
#include "cutscene.h"
#include "src/boot.h"
#include "src/graphics/renderscene.h"
#include "src/system/memory.h"
#include "src/level/level.h"
#include "src/audio/audio.h"
#include "src/cutscene/jpegdecoder.h"
#include "src/effects/tutorial.h"
#include "src/input/controller.h"
#include "slides.h"

#define CUTSCENE_HEIGHT     180
#define MAX_CUTSCENE_HEIGHT 480

#define MAX_JPEG_SIZE       (30*1024)

#define MAX_LOADED_SLIDES   2

#define SKIP_PROMPT_TIME    2.0f

struct CutscenePlayer gCutscenePlayer;
struct TimeUpdateListener gCutsceneListener;
char* gCutsceneBuffer[MAX_LOADED_SLIDES];
int gCurrentLoadedImage[MAX_LOADED_SLIDES];
char* gEncodedAddress;
float gSkipTimer;

short gPendingImageCount;
short gHasLoadedFirstImage; 

int cutsceneGetImageSlot(int slideId) {
    int i;
    for (i = 0; i < MAX_LOADED_SLIDES; ++i) {
        if (gCurrentLoadedImage[i] == slideId) {
            return i;
        }
    }

    return -1;
}

void cutSceneEnd() {
    timeRemoveListener(&gCutsceneListener, TimeUpdateGroupWorld);
    jpegDecoderFlush();
    levelSetNext(gCutscenePlayer.targetLevel, 1);
    audioStopSequence(0.0f);
}

int requestImageSlot(int nextSlide, int currentSlide) {
    int result = -1;
    int i;
    for (i = 0; i < MAX_LOADED_SLIDES; ++i) {
        if (gCurrentLoadedImage[i] == nextSlide) {
            return i;
        } else if (gCurrentLoadedImage[i] == -1 || gCurrentLoadedImage[i] != currentSlide) {
            result = i;
            break;
        }
    }

    if (result != -1) {
        gCurrentLoadedImage[result] = nextSlide;
        romCopy(
            gAllSlideLocations[nextSlide], 
            gEncodedAddress, 
            gAllSlideEndLocations[nextSlide] - gAllSlideLocations[nextSlide]
        );

        ++gPendingImageCount;
        jpegDecode(gEncodedAddress, gCutsceneBuffer[result]);
    }

    return -1;
}

struct CutsceneFrame* cutSceneGetFrame(int offset) {
    if (gCutscenePlayer.currentFrame+offset < gCutscenePlayer.cutscene->frameCount) {
        return &gCutscenePlayer.cutscene->frames[gCutscenePlayer.currentFrame+offset];
    }

    return 0;
}

void cutSceneUpdate(void* data) {
    struct JpegDecodeRequest* jpeg;
    if (osRecvMesg(&gCompletedImages, (OSMesg*)&jpeg, OS_MESG_NOBLOCK) == 0) {
        gHasLoadedFirstImage = 1;
        --gPendingImageCount;
    }

    if (!gHasLoadedFirstImage) {
        return;
    }

    if (getButtonDown(0, START_BUTTON)) {
        if (!tutorialMenuIsActive(&gTutorialMenu)) {
            tutorialMenuInit(&gTutorialMenu, TutorialMenuSkip);
            gSkipTimer = SKIP_PROMPT_TIME;
        } else if (gTutorialMenu.currTime == 0.0f) {
            cutSceneEnd();
        }
    }

    if (gSkipTimer > 0.0f) {
        gSkipTimer -= gTimeDelta;

        if (gSkipTimer <= 0.0f) {
            tutorialMenuClose(&gTutorialMenu);
        }
    }

    struct CutsceneFrame* currFrame = cutSceneGetFrame(0);
    struct CutsceneFrame* nextFrame = cutSceneGetFrame(1);

    if (nextFrame && cutsceneGetImageSlot(nextFrame->slideIndex) == -1) {
        requestImageSlot(nextFrame->slideIndex, currFrame->slideIndex);
    }

    if (currFrame && cutsceneGetImageSlot(currFrame->slideIndex) == -1) {
        return;
    }

    if (gCutscenePlayer.currentTime < currFrame->duration) {
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
                        audioStopSequence(currentEvent->seqStop.fadeTime);
                        break;
                    case CutsceneEventTypeSound:
                        audioPlaySound(
                            gPlayerSoundIds[currentEvent->sound.soundId],
                            currentEvent->sound.pitch,
                            currentEvent->sound.volume,
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
    } else {
        if (gCutscenePlayer.currentFrame + 1 >= gCutscenePlayer.cutscene->frameCount) {
            cutSceneEnd();
        } else if (gPendingImageCount == 0) {
            gCutscenePlayer.currentFrame++;
            gCutscenePlayer.currentTime = 0.0f;
        }
    }
}

char* cutsceneBufferAtY(char* input, int y) {
    return input + y * SCREEN_WD * 2;
}

#define MAX_TILE_X  64
#define MAX_TILE_Y  32

void graphicsCopyImage(struct GraphicsState* state, char* source, int iw, int ih, int sx, int sy, int dx, int dy, int width, int height, int alpha) {
    gDPPipeSync(state->dl++);
    gDPSetCycleType(state->dl++, G_CYC_1CYCLE);
    gDPSetRenderMode(state->dl++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
	gDPSetCombineLERP(state->dl++, 0, 0, 0, TEXEL0, 0, 0, 0, ENVIRONMENT, 0, 0, 0, TEXEL0, 0, 0, 0, ENVIRONMENT);
    gDPSetTextureLUT(state->dl++, G_TT_NONE);
    gDPSetTexturePersp(state->dl++, G_TP_NONE);
    gDPSetEnvColor(state->dl++, 255, 255, 255, alpha);

    int tileXCount = (width + MAX_TILE_X-1) / MAX_TILE_X;
    int tileYCount = (height + MAX_TILE_Y-1) / MAX_TILE_Y;
    int tileX, tileY;

    for (tileX = 0; tileX < tileXCount; ++tileX) {
        int currX = tileX * MAX_TILE_X;
        int tileWidth = width - currX;

        if (tileWidth > MAX_TILE_X) {
            tileWidth = MAX_TILE_X;
        }

        int currDx = dx + currX;

        if (currDx >= SCREEN_WD) {
            break;
        }

        if (currDx + tileWidth < 0) {
            continue;
        }

        if (currDx < 0) {
            tileWidth += currDx;
            currDx = 0;
        }

        if (currDx + tileWidth >= SCREEN_WD) {
            tileWidth = SCREEN_WD - currDx;
        }

        int scaledY = 0;

        for (tileY = 0; tileY < tileYCount; ++tileY) {
            int currY = tileY * MAX_TILE_Y;
            int tileHeight = height - currY;
            
            if (tileHeight > MAX_TILE_Y) {
                tileHeight = MAX_TILE_Y;
            }

            int scaledTileHeight = SCALE_FOR_PAL(tileHeight);
            
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
                (currDx) << 2, (dy+scaledY) << 2,
                (currDx+tileWidth) << 2, (dy+scaledY+scaledTileHeight) << 2,
                G_TX_RENDERTILE, 
                0, 0, 1 << 10, (tileHeight << 10) / scaledTileHeight
            );

            scaledY += scaledTileHeight;
        }
    }

    gDPPipeSync(state->dl++);
    gDPSetRenderMode(state->dl++, G_RM_AA_OPA_SURF, G_RM_AA_OPA_SURF2);
}

void cutSceneRender(void* data, struct GraphicsState* state, struct FontRenderer* fontRenderer) {
    if (gCutscenePlayer.currentFrame >= gCutscenePlayer.cutscene->frameCount || !gHasLoadedFirstImage) {
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

    int dxOffset = (int)(gCutscenePlayer.currentTime * currFrame->dxVelocity + currFrame->dxOffset);
    
    int barHeight = (SCREEN_HT_NTSC - CUTSCENE_HEIGHT) >> 1;

    int index = cutsceneGetImageSlot(currFrame->slideIndex);

    float alpha = 1.0f - currFrame->startFade - currFrame->fadeVelocity * gCutscenePlayer.currentTime;

    if (alpha < 0.0f) {
        alpha = 0.0f;
    } else if (alpha > 1.0f) {
        alpha = 1.0f;
    }

    if (index != -1) {
        graphicsCopyImage(state, gCutsceneBuffer[index], 
            SCREEN_WD, CUTSCENE_HEIGHT, 
            0, yOffset, 
            dxOffset, SCALE_FOR_PAL(barHeight), 
            SCREEN_WD, CUTSCENE_HEIGHT,
            (int)(255 * alpha)
        );
        gDPPipeSync(state->dl++);
    }
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

    gCurrentLoadedImage[0] = -1;
    gCurrentLoadedImage[1] = -1;

    gCutsceneBuffer[0] = heapMalloc(SCREEN_WD*MAX_CUTSCENE_HEIGHT*2,16);
    gCutsceneBuffer[1] = heapMalloc(SCREEN_WD*MAX_CUTSCENE_HEIGHT*2,16);

    renderSceneReset(&gScene);
    gScene.activeViewportCount = 0;

    gCutscenePlayer.cutscene = cutscene;
    gCutscenePlayer.currentFrame = 0;
    gCutscenePlayer.currentTime = 0.0f;
    gCutscenePlayer.totalTime = 0.0f;
    gCutscenePlayer.targetLevel = nextLevel;
    gCutscenePlayer.nextEvent = 0;

    gEncodedAddress = heapMalloc(MAX_JPEG_SIZE, 1);

    graphicsAddMenu(cutSceneRender, 0, 0);

    if (cutscene->soundBank != SoundBankNone) {
        playerSoundsUseBank(cutscene->soundBank);
    }

    jpegDecoderFlush();
    gPendingImageCount = 0;
    gHasLoadedFirstImage = 0;
    if (cutscene->frameCount) {
        requestImageSlot(cutscene->frames[0].slideIndex, -1);
    }

    timeAddListener(&gCutsceneListener, cutSceneUpdate, 0, TimeUpdateGroupWorld);
}