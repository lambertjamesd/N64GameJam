
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
#include "src/cadet/geo/model.h"
#include "src/math/basictransform.h"
#include "src/cadet/cadet.h"
#include "renderscene.h"

extern OSSched         gScheduler;
extern OSMesgQueue     *gSchedulerCommandQ;
extern GFXInfo         gInfo[];

u64 gRSPYieldBuffer[OS_YIELD_DATA_SIZE/sizeof(u64)];
// extra 64 bytes to make sure it is aligned to 64 bytes
unsigned short	gZBuffer[SCREEN_WD*SCREEN_HT + 64 / sizeof(u16)];
unsigned short* gColorBuffer[2];
Gfx* gStaticSegmentBuffer;
Gfx* gLevelSegmentBuffer;
Gfx* gLevelThemeSegmentBuffer;
u64 gDramStack[SP_DRAM_STACK_SIZE64];
Dynamic dynamic;

struct Vector3 cameraPos = {0.0f, 0.0f, 0.0f};

void graphicsInit(void) 
{    
    u32 len = (u32)(_staticSegmentRomEnd - _staticSegmentRomStart);

    gStaticSegmentBuffer = heapMalloc(len, 8);

    romCopy(_staticSegmentRomStart, (char*)gStaticSegmentBuffer, len);

    len = (u32)(_debuglevelSegmentRomEnd - _debuglevelSegmentRomStart);
    u32 levelAlign;
    u32 levelPageMask;
    tlbAlign(len, &levelAlign, &levelPageMask);

    gLevelSegmentBuffer = heapMalloc(len, levelAlign);
    romCopy(_debuglevelSegmentRomStart, (char*)gLevelSegmentBuffer, len);

    osUnmapTLBAll();
    osMapTLB(0, levelPageMask, (void*)(LEVEL_SEGMENT << 24), osVirtualToPhysical(gLevelSegmentBuffer), -1, -1);

    len = (u32)(_alienworldSegmentRomEnd - _alienworldSegmentRomStart);
    gLevelThemeSegmentBuffer = heapMalloc(len, 8);
    romCopy(_alienworldSegmentRomStart, (char*)gLevelThemeSegmentBuffer, len);

    gInfo[0].msg.gen.type = OS_SC_DONE_MSG;
    gInfo[0].cfb = gColorBuffer[0];
    gInfo[1].msg.gen.type = OS_SC_DONE_MSG;
    gInfo[1].cfb = gColorBuffer[1];

    osViSetSpecialFeatures(OS_VI_DITHER_FILTER_ON);
}

void createGfxTask(GFXInfo *i) 
{
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

    gSPDisplayList(glistp++, setup_rdpstate);

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

    gDPSetFillColor(glistp++, (GPACK_RGBA5551(gAlienWorldLevelTheme.clearColorR, gAlienWorldLevelTheme.clearColorG, gAlienWorldLevelTheme.clearColorB, 1) << 16 | 
			       GPACK_RGBA5551(gAlienWorldLevelTheme.clearColorR, gAlienWorldLevelTheme.clearColorG, gAlienWorldLevelTheme.clearColorB, 1)));
    gDPFillRectangle(glistp++, 0, 0, SCREEN_WD-1, SCREEN_HT-1);

    gDPPipeSync(glistp++);
    gDPSetCycleType(glistp++, G_CYC_1CYCLE);

    gSPClearGeometryMode(glistp++, G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR | G_CULL_FRONT | G_FOG | G_LIGHTING | G_SHADE);
    gSPSetGeometryMode(glistp++, G_ZBUFFER | G_SHADING_SMOOTH | G_CULL_BACK);
	gDPSetTextureLUT(glistp++, G_TT_NONE);

    guPerspective(&dynamicp->projection, &dynamicp->perspectiveCorrect, 70.0f, 4.0f / 3.0f, 1.0f, 128.0f, 1.0f);
    gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&dynamicp->projection), G_MTX_PROJECTION|G_MTX_LOAD|G_MTX_NOPUSH);

    cameraPos.x += gControllerState[0].stick_x / (80.0f * 30.0f);
    cameraPos.z -= gControllerState[0].stick_y / (80.0f * 30.0f);

    struct BasicTransform cameraInverse;
    float mtx[4][4];
    transformInvert(&gScene.camera.transform, &cameraInverse);
    transformToMatrix(&cameraInverse, mtx);
    guScale(&dynamicp->worldScale, 1.0f / 256.0f, 1.0f / 256.0f, 1.0f / 256.0f);
    guMtxF2L(mtx, &dynamicp->viewing);

    gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&dynamicp->viewing), G_MTX_MODELVIEW|G_MTX_LOAD|G_MTX_NOPUSH);

    gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&dynamicp->worldScale), G_MTX_MODELVIEW|G_MTX_MUL|G_MTX_PUSH);
    glistp = graphicsRenderLevelTileGrid(&_level_debug_levelGraphics.grid, gAlienWorldLevelTheme.materials, gAlienWorldLevelTheme.materialCount, glistp);
    gSPPopMatrix(glistp++, G_MTX_MODELVIEW);

    transformToMatrixL(&gCadet.transform, &dynamicp->cadet);
    gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&dynamicp->cadet), G_MTX_MODELVIEW|G_MTX_MUL|G_MTX_PUSH);
    gSPDisplayList(glistp++, Cadet_Cadet_mesh);
    gSPPopMatrix(glistp++, G_MTX_MODELVIEW);

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