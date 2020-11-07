
#include <ultralog.h>

#include "graphics.h"
#include "src/boot.h"
#include "src/system/memory.h"
#include "src/levels/debug/header.h"
#include "src/levelthemes/alienworld/theme.h"
#include "src/input/controller.h"
#include "src/math/vector.h"
#include "levelgraphics.h"
#include "src/system/assert.h"
#include "src/math/basictransform.h"
#include "src/level/level.h"
#include "renderscene.h"

extern OSSched         gScheduler;
extern OSMesgQueue     *gSchedulerCommandQ;
extern GFXInfo         gInfo[];

u64 gRSPYieldBuffer[OS_YIELD_DATA_SIZE/sizeof(u64)];
// extra 64 bytes to make sure it is aligned to 64 bytes
unsigned short	gZBuffer[SCREEN_WD*SCREEN_HT + 64 / sizeof(u16)];
unsigned short* gColorBuffer[2];
char* gStaticSegmentBuffer;
char* gLevelSegmentBuffer;
char* gLevelThemeSegmentBuffer;
struct LevelGraphics* gCurrentLevelGraphics;
struct LevelThemeGraphics* gCurrentLevelTheme;
u64 gDramStack[SP_DRAM_STACK_SIZE64];

void graphicsInit(void) 
{
    gInfo[0].msg.gen.type = OS_SC_DONE_MSG;
    gInfo[0].cfb = gColorBuffer[0];
    gInfo[1].msg.gen.type = OS_SC_DONE_MSG;
    gInfo[1].cfb = gColorBuffer[1];

    osViSetSpecialFeatures(OS_VI_DITHER_FILTER_ON);
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
        gCurrentLevelTheme->animCallback();
    }

    static int firsttime = 1;
    Dynamic *dynamicp;
    OSScTask *t;
    
    dynamicp = &i->dp;
    glistp   = i->dp.glist;

    gSPSegment(glistp++, 0, 0);
    gSPSegment(glistp++, STATIC_SEGMENT,  osVirtualToPhysical(gStaticSegmentBuffer));
    gSPSegment(glistp++, DYNAMIC_SEGMENT, osVirtualToPhysical(dynamicp));
    gSPSegment(glistp++, LEVEL_SEGMENT, osVirtualToPhysical(gLevelSegmentBuffer));
    gSPSegment(glistp++, LEVEL_THEME_SEGMENT, osVirtualToPhysical(gLevelThemeSegmentBuffer));

    gSPDisplayList(glistp++, setup_rspstate);
    if (firsttime) {
        gSPDisplayList(glistp++, rdpstateinit_dl);
	    firsttime = 0;
    }

    u32 zBuffAligned = ALIGN_64_BYTES(osVirtualToPhysical(gZBuffer));
    
    gDPSetDepthImage(glistp++, zBuffAligned);
    gDPPipeSync(glistp++);
    gDPSetCycleType(glistp++, G_CYC_FILL);
    gDPSetColorImage(glistp++, G_IM_FMT_RGBA, G_IM_SIZ_16b, SCREEN_WD, zBuffAligned);
    gDPSetFillColor(glistp++, (GPACK_ZDZ(G_MAXFBZ,0) << 16 | GPACK_ZDZ(G_MAXFBZ,0)));
    gDPFillRectangle(glistp++, 0, 0, SCREEN_WD-1, SCREEN_HT-1);
	
    gDPPipeSync(glistp++);
    gDPSetColorImage(glistp++, G_IM_FMT_RGBA, G_IM_SIZ_16b, SCREEN_WD,
		     osVirtualToPhysical(i->cfb));

    if (gCurrentLevelTheme) {
        gSPDisplayList(glistp++, gCurrentLevelTheme->clear);
    } else {
        gDPSetFillColor(glistp++, (GPACK_RGBA5551(0, 0, 0, 1) << 16 | 
                    GPACK_RGBA5551(0, 0, 0, 1)));
        gDPFillRectangle(glistp++, 0, 0, SCREEN_WD-1, SCREEN_HT-1);
    }

    gDPPipeSync(glistp++);
    gDPSetCycleType(glistp++, G_CYC_1CYCLE);

    gSPClearGeometryMode(glistp++, G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR | G_CULL_FRONT | G_FOG | G_LIGHTING | G_SHADE);
    gSPSetGeometryMode(glistp++, G_ZBUFFER | G_SHADING_SMOOTH | G_CULL_BACK);
	gDPSetTextureLUT(glistp++, G_TT_NONE);

    guPerspective(&dynamicp->projection, &dynamicp->perspectiveCorrect, 70.0f, 4.0f / 3.0f, 1.0f, 128.0f, 1.0f);
    gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&dynamicp->projection), G_MTX_PROJECTION|G_MTX_LOAD|G_MTX_NOPUSH);

    struct BasicTransform cameraInverse;
    transformInvert(&gScene.camera.transform, &cameraInverse);
    transformToMatrixL(&cameraInverse, 1.0f, &dynamicp->viewing);
    guScale(&dynamicp->worldScale, 1.0f / 256.0f, 1.0f / 256.0f, 1.0f / 256.0f);

    gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&dynamicp->viewing), G_MTX_MODELVIEW|G_MTX_LOAD|G_MTX_NOPUSH);

    if (gCurrentLevelGraphics && gCurrentLevelTheme) {
        gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&dynamicp->worldScale), G_MTX_MODELVIEW|G_MTX_MUL|G_MTX_PUSH);
        glistp = graphicsRenderLevelTileGrid(&gCurrentLevelGraphics->grid, gCurrentLevelTheme->materials, gCurrentLevelTheme->materialCount, glistp);
        gSPPopMatrix(glistp++, G_MTX_MODELVIEW);

        gSPClearGeometryMode(glistp++, G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR | G_CULL_FRONT | G_FOG | G_LIGHTING | G_SHADE);
        gSPSetGeometryMode(glistp++, G_ZBUFFER | G_SHADING_SMOOTH | G_CULL_BACK);
        gDPSetRenderMode(glistp++, G_RM_ZB_OPA_SURF, G_RM_ZB_OPA_SURF2);
	    gDPSetTextureLUT(glistp++, G_TT_NONE);
    }

    struct GraphicsState state;
    state.dl = glistp;
    state.matrices = dynamicp->dynamicActors;
    state.usedMatrices = 0;
    state.matrixCount = MAX_DYNAMIC_ACTORS;
    state.primColor = 0;

    if (gCurrentLevelTheme) {
        dynamicActorGroupRender(&gScene.dynamicActors, &state, gCurrentLevelTheme->dynamicMaterials, gCurrentLevelTheme->dynamicMaterialCount);
    } else {
        dynamicActorGroupRender(&gScene.dynamicActors, &state, 0, 0);
    }

    glistp = state.dl;

    gDPFullSync(glistp++);
    gSPEndDisplayList(glistp++);

    osWritebackDCache(&i->dp, (s32)glistp - (s32)&i->dp);
    osWritebackDCache(&dynamicp, sizeof(Dynamic));

    assert(glistp <= &dynamicp->glist[DYANAMIC_LIST_LEN]);

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
}