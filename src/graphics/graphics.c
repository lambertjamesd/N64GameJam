
#include <ultralog.h>

#include "graphics.h"
#include "src/boot.h"
#include "src/system/memory.h"
#include "src/levelthemes/alienworld/theme.h"
#include "src/input/controller.h"
#include "src/math/vector.h"
#include "levelgraphics.h"
#include "src/system/assert.h"
#include "src/math/basictransform.h"
#include "src/level/level.h"
#include "renderscene.h"
#include "src/font/endlessbossbattle/endlessbossbattle.h"
#include "src/debugger/debugger.h"

#if VALIDATE_GFX
#include "src/gfxvalidator/validator.h"
#endif

#define MAX_MENUS   8

extern OSSched         gScheduler;
extern OSMesgQueue     *gSchedulerCommandQ;
extern GFXInfo         gInfo[];

int gScreenHeight;
float gScreenYScale;
short gUnusedDL;
short gUnusedMatrices;

u64 __attribute__((aligned(64))) gRSPYieldBuffer[OS_YIELD_DATA_SIZE/sizeof(u64)];
unsigned short __attribute__((aligned(64)))	gZBuffer[SCREEN_WD*MAX_SCREEN_HT];
unsigned short* gColorBuffer[2];
char* gStaticSegmentBuffer;
char* gLevelSegmentBuffer;
char* gLevelThemeSegmentBuffer;
struct LevelGraphics* gCurrentLevelGraphics;
struct LevelThemeGraphics* gCurrentLevelTheme;
u64 gDramStack[SP_DRAM_STACK_SIZE64];
MenuRenderCallback gMenuGraphics[MAX_MENUS];
void* gMenuGraphicsData[MAX_MENUS];

static Vp gFullVP = {
    SCREEN_WD*2, 240*2, G_MAXZ/2, 0,
    SCREEN_WD*2, 240*2, G_MAXZ/2, 0,
};

static Vp gFullPalVP = {
    SCREEN_WD*2, 288*2, G_MAXZ/2, 0,
    SCREEN_WD*2, 288*2, G_MAXZ/2, 0,
};

void graphicsInit(void) 
{
    gInfo[0].msg.gen.type = OS_SC_DONE_MSG;
    gInfo[0].cfb = gColorBuffer[0];
    gInfo[1].msg.gen.type = OS_SC_DONE_MSG;
    gInfo[1].cfb = gColorBuffer[1];

	switch (osTvType) {
		case 0: // PAL
			osViSetMode(&osViModeTable[OS_VI_FPAL_LPF1]);
            gScreenHeight = SCREEN_HT_PAL;
            gScreenYScale = (float)SCREEN_HT_PAL/SCREEN_HT_NTSC;
			break;
		case 1: // NTSC
			osViSetMode(&osViModeTable[OS_VI_NTSC_LPF1]);
            gScreenHeight = SCREEN_HT_NTSC;
            gScreenYScale = 1.0f;
			break;
		case 2: // MPAL
			osViSetMode(&osViModeTable[OS_VI_MPAL_LPF1]);
            gScreenHeight = SCREEN_HT_NTSC;
            gScreenYScale = 1.0f;
			break;
	}

	osViSetSpecialFeatures(OS_VI_GAMMA_OFF |
			OS_VI_GAMMA_DITHER_OFF |
			OS_VI_DIVOT_OFF |
			OS_VI_DITHER_FILTER_OFF);
}

void graphicsCaluclateFrustum(Dynamic* dynamicp, struct SceneViewport* vp, int cameraIndex, struct Plane out[4]) {
    float perspectiveMtx[4][4];
    float viewMtx[4][4];
    struct BasicTransform cameraInverse;

    guPerspectiveF(
        perspectiveMtx, 
        &dynamicp->perspectiveCorrect[cameraIndex], 
        gScene.fov[cameraIndex], 
        gScreenYScale * (float)(vp->maxx - vp->minx) / (float)(vp->maxy - vp->miny),
        1.0f, 
        128.0f, 
        1.0f
    );

    guMtxF2L(perspectiveMtx, &dynamicp->projection[cameraIndex]);

    transformInvert(&gScene.camera[cameraIndex].transform, &cameraInverse);
    transformToMatrix(&cameraInverse, 1.0f, viewMtx);
    guMtxF2L(viewMtx, &dynamicp->viewing[cameraIndex]);

    guMtxCatF(viewMtx, perspectiveMtx, perspectiveMtx);
    planeFrustumExtractFromMtx(perspectiveMtx, out);
}

void graphicsInitLevel(
    char* staticSegment, 
    char* levelSegment, 
    char* themeSegment, 
    struct LevelGraphics* levelGraphics, 
    struct LevelThemeGraphics* theme
) {
    gStaticSegmentBuffer = staticSegment;
    gLevelSegmentBuffer = levelSegment;
    gLevelThemeSegmentBuffer = themeSegment;

    gCurrentLevelGraphics = levelGraphics;
    gCurrentLevelTheme = theme;
}

void createGfxTask(GFXInfo *i) {
    if (gCurrentLevelTheme && gCurrentLevelTheme->animCallback) {
        gCurrentLevelTheme->animCallback(gLevelThemeSegmentBuffer);
    }

    static int firsttime = 1;
    Dynamic *dynamicp;
    OSScTask *t;
    
    dynamicp = &i->dp;
    Gfx* glistp   = i->dp.glist;

    if (gCurrentLevel == SceneIndexMainMenu) {
        // gdbSetWatchPoint(((char*)&gInfo) + 736, 1, 1);
        // gdbSetWatchPoint(((char*)&gInfo) + 20240, 1, 1);
    }

    gSPSegment(glistp++, 0, 0);
    if (gStaticSegmentBuffer) {
        gSPSegment(glistp++, STATIC_SEGMENT,  osVirtualToPhysical(gStaticSegmentBuffer));
    }
    gSPSegment(glistp++, DYNAMIC_SEGMENT, osVirtualToPhysical(dynamicp));
    if (gLevelSegmentBuffer) {
        gSPSegment(glistp++, LEVEL_SEGMENT, osVirtualToPhysical(gLevelSegmentBuffer));
    }
    if (gLevelThemeSegmentBuffer) {
        gSPSegment(glistp++, LEVEL_THEME_SEGMENT, osVirtualToPhysical(gLevelThemeSegmentBuffer));
    }

    gSPDisplayList(glistp++, setup_rspstate);
    if (firsttime) {
        if (gScreenHeight == SCREEN_HT_PAL) {
            gSPDisplayList(glistp++, rdpstateinit_dl_pal);
        } else {
            gSPDisplayList(glistp++, rdpstateinit_dl);
        }
	    firsttime = 0;
    }

    u32 zBuffAligned = osVirtualToPhysical(gZBuffer);
    
    gDPSetDepthImage(glistp++, zBuffAligned);
    gDPPipeSync(glistp++);
    gDPSetCycleType(glistp++, G_CYC_FILL);
    gDPSetColorImage(glistp++, G_IM_FMT_RGBA, G_IM_SIZ_16b, SCREEN_WD, zBuffAligned);
    gDPSetFillColor(glistp++, (GPACK_ZDZ(G_MAXFBZ,0) << 16 | GPACK_ZDZ(G_MAXFBZ,0)));
    gDPFillRectangle(glistp++, 0, 0, SCREEN_WD-1, gScreenHeight-1);
	
    gDPPipeSync(glistp++);
    gDPSetColorImage(glistp++, G_IM_FMT_RGBA, G_IM_SIZ_16b, SCREEN_WD,
		     osVirtualToPhysical(i->cfb));

    if (gCurrentLevelTheme) {
        if (gScreenHeight == SCREEN_HT_PAL) {
            gSPDisplayList(glistp++, gCurrentLevelTheme->clearPal);
        } else {
            gSPDisplayList(glistp++, gCurrentLevelTheme->clear);
        }
    } else {
        gDPSetFillColor(glistp++, (GPACK_RGBA5551(0, 0, 0, 1) << 16 | 
                    GPACK_RGBA5551(0, 0, 0, 1)));
        gDPFillRectangle(glistp++, 0, 0, SCREEN_WD-1, gScreenHeight-1);
    }

    struct GraphicsState state;
    state.dl = glistp;
    state.matrices = dynamicp->dynamicActors;
    state.usedMatrices = 0;
    state.matrixCount = DYNAMIC_MATRIX_COUNT;
    state.primColor = 0;
    state.lookats = dynamicp->lookats;
    state.usedLookAt = 0;
    state.lookAtCount = LOOK_AT_COUNT;
    state.cfb = i->cfb;

    u16* buffer = i->cfb;

    int index;
    for (index = 0; index < gScene.activeViewportCount && index < MAX_VIEWPORTS; ++index) {
        if (gScene.viewports[index].minx >= gScene.viewports[index].maxx || 
            gScene.viewports[index].miny >= gScene.viewports[index].maxy) {
            continue;
        }

        state.cameraPos = &gScene.camera[index].transform.position;

        gDPPipeSync(state.dl++);
        gDPSetCycleType(state.dl++, G_CYC_1CYCLE);

        gSPClearGeometryMode(state.dl++, G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR | G_CULL_FRONT | G_FOG | G_LIGHTING | G_SHADE);
        gSPSetGeometryMode(state.dl++, G_ZBUFFER | G_SHADING_SMOOTH | G_CULL_BACK);
        gDPSetRenderMode(state.dl++, G_RM_ZB_OPA_SURF, G_RM_ZB_OPA_SURF2);
        gDPSetTextureLUT(state.dl++, G_TT_NONE);

        struct SceneViewport* vp = &gScene.viewports[index];

        viewportConvert(vp, &dynamicp->viewports[index]);

        gSPViewport(state.dl++, K0_TO_PHYS(&dynamicp->viewports[index]));

        graphicsCaluclateFrustum(dynamicp, vp, index, state.frustumPlanes);
        guScale(&dynamicp->worldScale[index], 1.0f / 16.0f, 1.0f / 16.0f, 1.0f / 16.0f);

        gSPMatrix(state.dl++, OS_K0_TO_PHYSICAL(&dynamicp->projection[index]), G_MTX_PROJECTION|G_MTX_LOAD|G_MTX_NOPUSH);
        gSPMatrix(state.dl++, OS_K0_TO_PHYSICAL(&dynamicp->viewing[index]), G_MTX_MODELVIEW|G_MTX_LOAD|G_MTX_NOPUSH);

        if (gCurrentLevelTheme) {
            dynamicActorGroupRender(&gScene.dynamicActors, &state, gCurrentLevelTheme->dynamicMaterials, gCurrentLevelTheme->dynamicMaterialCleanup, gCurrentLevelTheme->dynamicMaterialCount);
        } else {
            dynamicActorGroupRender(&gScene.dynamicActors, &state, 0, 0, 0);
        }

        gSPClearGeometryMode(state.dl++, G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR | G_CULL_FRONT | G_FOG | G_LIGHTING | G_SHADE);
        gSPSetGeometryMode(state.dl++, G_ZBUFFER | G_SHADING_SMOOTH | G_CULL_BACK);

        if (gCurrentLevelGraphics && gCurrentLevelTheme) {
            gSPMatrix(state.dl++, OS_K0_TO_PHYSICAL(&dynamicp->worldScale[index]), G_MTX_MODELVIEW|G_MTX_MUL|G_MTX_PUSH);
            graphicsRenderLevelTileGrid(&gCurrentLevelGraphics->grid, gCurrentLevelTheme->materials, gCurrentLevelTheme->materialCount, &state);
            gSPPopMatrix(state.dl++, G_MTX_MODELVIEW);

            gDPPipeSync(state.dl++);

            gSPClearGeometryMode(state.dl++, G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR | G_CULL_FRONT | G_FOG | G_LIGHTING | G_SHADE);
            gSPSetGeometryMode(state.dl++, G_ZBUFFER | G_SHADING_SMOOTH | G_CULL_BACK);
            gDPSetRenderMode(state.dl++, G_RM_ZB_OPA_SURF, G_RM_ZB_OPA_SURF2);
            gDPSetTextureLUT(state.dl++, G_TT_NONE);
        }

	    gDPSetRenderMode(state.dl++, G_RM_AA_ZB_XLU_SURF, G_RM_AA_ZB_XLU_SURF2);

        dynamicActorGroupRender(&gScene.transparentActors, &state, gScene.transparentMaterials, gScene.transparentMaterialCleanup, MAX_MATERIAL_GROUPS);

    }
    
    glistp = state.dl;

    gDPPipeSync(glistp++);
    if (gScreenHeight == SCREEN_HT_PAL) {
        gSPViewport(glistp++, K0_TO_PHYS(&gFullPalVP));
    } else {
        gSPViewport(glistp++, K0_TO_PHYS(&gFullVP));
    }

    gSPClearGeometryMode(state.dl++, G_ZBUFFER|G_CULL_BACK);
    gDPSetRenderMode(state.dl++, G_RM_OPA_SURF, G_RM_OPA_SURF2);

    if (gScene.activeViewportCount == 2 && 
        gScene.viewports[0].maxx < gScene.viewports[1].minx &&
        gScene.viewports[0].maxx >=0 && gScene.viewports[1].minx <= SCREEN_WD) {
        gDPSetCycleType(glistp++, G_CYC_FILL);
        gDPSetFillColor(glistp++, (GPACK_RGBA5551(0, 0, 0, 1) << 16 | 
                    GPACK_RGBA5551(0, 0, 0, 1)));
        gDPFillRectangle(
            glistp++, 
            gScene.viewports[0].maxx,
            0,
            gScene.viewports[1].minx-1,
            gScreenHeight-1
        );
        gDPPipeSync(glistp++);
        gDPSetCycleType(glistp++, G_CYC_1CYCLE);
    }

    state.dl = glistp;

    fontRendererBeginFrame(&dynamicp->fontRenderer);

    for (index = 0; index < MAX_MENUS; ++index) {
        if (gMenuGraphics[index]) {
            gMenuGraphics[index](gMenuGraphicsData[index], &state, &dynamicp->fontRenderer);
        }
    }

    glistp = state.dl;

    gDPFullSync(glistp++);
    gSPEndDisplayList(glistp++);

    osWritebackDCache(&i->dp, (s32)glistp - (s32)&i->dp);
    osWritebackDCache(&dynamicp, sizeof(Dynamic));

    assert(glistp <= &dynamicp->glist[DYANAMIC_LIST_LEN]);

    gUnusedDL = &dynamicp->glist[DYANAMIC_LIST_LEN] - glistp;
    gUnusedMatrices = state.matrixCount - state.usedMatrices;

    t = &i->task;
    t->list.t.data_ptr = (u64 *) dynamicp->glist;
    t->list.t.data_size = (s32)(glistp - dynamicp->glist) * sizeof (Gfx);
    t->list.t.type = M_GFXTASK;
    t->list.t.flags = 0x0;
    t->list.t.ucode_boot = (u64 *)rspbootTextStart;
    t->list.t.ucode_boot_size = ((s32) rspbootTextEnd - 
				 (s32) rspbootTextStart);
    t->list.t.ucode = (u64 *) gspFast3DTextStart;
    t->list.t.ucode_data = (u64 *) gspFast3DDataStart;
    t->list.t.ucode_data_size = SP_UCODE_DATA_SIZE;
    t->list.t.dram_stack = (u64 *) gDramStack;
    t->list.t.dram_stack_size = SP_DRAM_STACK_SIZE8;
    t->list.t.output_buff = (u64 *) 0x0;
    t->list.t.output_buff_size = (u64 *) 0x0;
    t->list.t.yield_data_ptr = (u64 *) gRSPYieldBuffer;
    t->list.t.yield_data_size = OS_YIELD_DATA_SIZE;

    t->next     = 0;
    t->flags	= (OS_SC_NEEDS_RSP | OS_SC_NEEDS_RDP | OS_SC_LAST_TASK |
		   OS_SC_SWAPBUFFER);
    t->msgQ     = &gGfxFrameMsgQ;
    t->msg      = (OSMesg)&i->msg;
    t->framebuffer = (void *)i->cfb;
    osSendMesg(gSchedulerCommandQ, (OSMesg) t, OS_MESG_BLOCK); 

#if VALIDATE_GFX
    struct GFXValidationResult validateResult;
    enum GFXValidatorError error = gfxValidate(&t->list, DYANAMIC_LIST_LEN, &validateResult);
    assert(!error);
#endif
}

void graphicsAddMenu(MenuRenderCallback renderCallback, void* data, int priority) {
    int i;
    if (priority) {
        for (i = MAX_MENUS-1; i >= 0; --i) {
            if (!gMenuGraphics[i]) {
                gMenuGraphics[i] = renderCallback;
                gMenuGraphicsData[i] = data;
                break;
            }
        }
    } else {
        for (i = 0; i < MAX_MENUS; ++i) {
            if (!gMenuGraphics[i]) {
                gMenuGraphics[i] = renderCallback;
                gMenuGraphicsData[i] = data;
                break;
            }
        }
    }
}

void graphicsRemoveMenu(MenuRenderCallback renderCallback, void* data) {
    int i;
    for (i = 0; i < MAX_MENUS; ++i) {
        if (gMenuGraphics[i] == renderCallback && gMenuGraphicsData[i] == data) {
            gMenuGraphics[i] = 0;
            gMenuGraphicsData[i] = 0;
            break;
        }
    }
}

void graphicsClearMenus() {
    int i;
    for (i = 0; i < MAX_MENUS; ++i) {
        gMenuGraphics[i] = 0;
        gMenuGraphicsData[i] = 0;
    }
}