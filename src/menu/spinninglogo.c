
#include "spinninglogo.h"

#include <ultra64.h>

#include "src/system/memory.h"
#include "src/boot.h"
#include "src/cadet/cadet.h"
#include "src/font/font.h"
#include "src/audio/audio.h"
#include "src/audio/allseq.h"

#define SPINNING_LOGO_TIME      8.0f
#define FADE_TIME               2.0f
#define JINGLE_PLAY_TIME        1.0f
#define DELAY_TIME              1.0f
#define RADS_PER_SEC            (M_PI*1/3)
#define RAD_OFFSET              (M_PI*0.5f)

extern struct FontCharacter gFreshEaterCharacters[];
extern struct Font gFreshEater;
extern Gfx gFreshEaterUse[];

extern Gfx SpinningLogo_SpinningLogo_mesh[];
extern char _spinning_logoSegmentRomStart[], _spinning_logoSegmentRomEnd[];

struct TimeUpdateListener gSpinningLogoUpdate;
float gSpinningLogoTimer;

void spinningLogoText(void* data, struct GraphicsState* state, struct FontRenderer* fontRenderer) {
    gDPPipeSync(state->dl++);
    gDPSetCycleType(state->dl++, G_CYC_1CYCLE);

    gSPDisplayList(state->dl++, gFreshEaterUse);
    fontRendererDrawCharacters(
        fontRenderer,
        &gFreshEater,
        &state->dl,
        "ULTRA RARE",
        80,
        163
    );

    u8 alpha = 255;

    if (gSpinningLogoTimer < DELAY_TIME) {
        alpha = 0;
    } else if (gSpinningLogoTimer < DELAY_TIME + FADE_TIME) {
        alpha = (u8)(255 * (gSpinningLogoTimer - DELAY_TIME) / FADE_TIME);
    } else if (gSpinningLogoTimer >= SPINNING_LOGO_TIME) {
        alpha = 0;
    } else if (gSpinningLogoTimer > SPINNING_LOGO_TIME - FADE_TIME) {
        alpha = (u8)(255 * (SPINNING_LOGO_TIME - gSpinningLogoTimer) / FADE_TIME);
    }

    gDPPipeSync(state->dl++);
    gDPSetCombineLERP(state->dl++, 0, 0, 0, ENVIRONMENT, 0, 0, 0, ENVIRONMENT, 0, 0, 0, ENVIRONMENT, 0, 0, 0, ENVIRONMENT);
    gDPSetEnvColor(state->dl++, 0, 0, 0, 255 - alpha);
    gDPFillRectangle(state->dl++, 0, 0, SCREEN_WD-1, SCREEN_HT-1);
    gDPPipeSync(state->dl++);
}

void spinningLogoRender(struct DynamicActor* data, struct GraphicsState* state) {
    struct BasicTransform spinningTransform;
    spinningTransform.position = gZeroVec;
    spinningTransform.scale = 1.0f;
    quatAxisAngle(
        &gUp, 
        -gSpinningLogoTimer * RADS_PER_SEC + RAD_OFFSET, 
        &spinningTransform.rotation
    );

    Mtx* nextTransfrom = graphicsStateNextMtx(state);
    transformToMatrixL(&spinningTransform, 1.0f / 256.0f, nextTransfrom);
    gSPSetGeometryMode(state->dl++, G_ZBUFFER);
    gDPSetRenderMode(state->dl++, G_RM_ZB_OPA_SURF, G_RM_ZB_OPA_SURF2);
    gSPMatrix(state->dl++, OS_K0_TO_PHYSICAL(nextTransfrom), G_MTX_MODELVIEW|G_MTX_MUL|G_MTX_PUSH);
    gSPDisplayList(state->dl++, SpinningLogo_SpinningLogo_mesh);
    gSPPopMatrix(state->dl++, G_MTX_MODELVIEW);
}

void spinningLogoUpdate(void* data) {
    if (gSpinningLogoTimer >= SPINNING_LOGO_TIME) {
        gNextLevel = SceneIndexMainMenu;
    } else {
        float nextTime = gSpinningLogoTimer + gTimeDelta;

        if (nextTime >= JINGLE_PLAY_TIME && gSpinningLogoTimer < JINGLE_PLAY_TIME) {
            struct SeqPlayEvent playEvent;
            playEvent.romStart = _logoJingleSegmentRomStart;
            playEvent.romEnd = _logoJingleSegmentRomEnd;
            playEvent.loopStart = 0;
            playEvent.loopEnd = 0;
            playEvent.loopCount = 0;
            playEvent.playbackStart = 0;
            audioPlaySequence(&playEvent);
        }

        gSpinningLogoTimer = nextTime;
    }
}

void spinningLogoInit() {
    heapReset();
    osUnmapTLBAll();

    u32 len = (u32)(_staticSegmentRomEnd - _staticSegmentRomStart);
    char* staticSegment = heapMalloc(len, 8);
    romCopy(_staticSegmentRomStart, (char*)staticSegment, len);

    len = (u32)(_spinning_logoSegmentRomEnd - _spinning_logoSegmentRomStart);
    u32 levelAlign;
    u32 levelPageMask;
    tlbAlign(len, &levelAlign, &levelPageMask);

    char* levelSegment = heapMalloc(len, levelAlign);
    romCopy(_spinning_logoSegmentRomStart, levelSegment, len);
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

    gScene.camera[0].transform.position.x = 0;
    gScene.camera[0].transform.position.y = 2;
    gScene.camera[0].transform.position.z = 5;

    quatAxisAngle(&gRight, -27.0f * (M_PI / 180.0f), &gScene.camera[0].transform.rotation);

    gScene.camera[0].transform.scale = 1.0f;

    gScene.viewports[0].minx = 0;
    gScene.viewports[0].maxx = SCREEN_WD;
    gScene.viewports[0].miny = 0;
    gScene.viewports[0].maxy = SCREEN_HT;

    gScene.fov[0] = 70.0f;

    gSpinningLogoTimer = 0.0f;

    fontInit(&gFreshEater, gFreshEaterCharacters, gFreshEaterUse, 30);

    timeAddListener(&gSpinningLogoUpdate, spinningLogoUpdate, 0, TimeUpdateGroupWorld);
    
    dynamicActorAddToGroup(&gScene.dynamicActors, &gCadet.transform, 0, spinningLogoRender, MATERIAL_INDEX_NOT_BATCHED, 1.0f);

    graphicsAddMenu(spinningLogoText, 0, 1);
}