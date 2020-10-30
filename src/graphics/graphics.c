
#include <ultralog.h>

#include "graphics.h"
#include "src/boot.h"
#include "src/memory.h"
#include "src/levels/debug/header.h"
#include "src/levelthemes/alienworld/header.h"
#include "src/input/controller.h"
#include "src/math/vector.h"

extern OSSched         gScheduler;
extern OSMesgQueue     *gSchedulerCommandQ;
extern GFXInfo         gInfo[];

u64 gRSPYieldBuffer[OS_YIELD_DATA_SIZE/sizeof(u64)];
unsigned short	gZBuffer[SCREEN_WD*SCREEN_HT];
unsigned short* gColorBuffer[2];
Gfx* gStaticSegmentBuffer;
Gfx* gLevelSegmentBuffer;
Gfx* gLevelThemeSegmentBuffer;
u64 gDramStack[SP_DRAM_STACK_SIZE64];
Dynamic dynamic;

struct Vector3 cameraPos = {15.0f, 0.0f, 0.0f};

void graphicsInit(void) 
{    
    u32 len = (u32)(_staticSegmentRomEnd - _staticSegmentRomStart);

    gStaticSegmentBuffer = heapMalloc(len, 8);

    romCopy(_staticSegmentRomStart, (char*)gStaticSegmentBuffer, len);

    len = (u32)(_debuglevelSegmentRomEnd - _debuglevelSegmentRomStart);
    gLevelSegmentBuffer = heapMalloc(len, 8);
    romCopy(_debuglevelSegmentRomStart, (char*)gLevelSegmentBuffer, len);

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
    
    gDPSetDepthImage(glistp++, osVirtualToPhysical(gZBuffer));
    gDPPipeSync(glistp++);
    gDPSetCycleType(glistp++, G_CYC_FILL);
    gDPSetColorImage(glistp++, G_IM_FMT_RGBA, G_IM_SIZ_16b, SCREEN_WD,
		     osVirtualToPhysical(gZBuffer));
    gDPSetFillColor(glistp++, (GPACK_ZDZ(G_MAXFBZ,0) << 16 |
			       GPACK_ZDZ(G_MAXFBZ,0)));
    gDPFillRectangle(glistp++, 0, 0, SCREEN_WD-1, SCREEN_HT-1);
	
    gDPPipeSync(glistp++);
    gDPSetColorImage(glistp++, G_IM_FMT_RGBA, G_IM_SIZ_16b, SCREEN_WD,
		     osVirtualToPhysical(i->cfb));

    gDPSetFillColor(glistp++, (GPACK_RGBA5551(46, 14, 89, 1) << 16 | 
			       GPACK_RGBA5551(46, 14, 89, 1)));
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

    Mtx worldScale;
    Mtx traslate;
    Mtx rotate;
    Mtx combine;
    guScale(&worldScale, 1.0f / 256.0f, 1.0f / 256.0f, 1.0f / 256.0f);
    guTranslate(&traslate, -cameraPos.x, -10.0f, -cameraPos.z);
    guRotate(&rotate, 60.0f, 1.0f, 0.0f, 0.0f);
    guMtxCatL(&worldScale, &traslate, &combine);
    guMtxCatL(&combine, &rotate, &dynamicp->viewing);
    gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&dynamicp->viewing), G_MTX_MODELVIEW|G_MTX_LOAD|G_MTX_NOPUSH);

    // gSPDisplayList(glistp++, _alienFloor_material);
    // gSPDisplayList(glistp++, _level_debug_geo_0_tri);
    // gSPDisplayList(glistp++, _level_debug_geo_1_tri);
    // gSPDisplayList(glistp++, _level_debug_geo_2_tri);
    // gSPDisplayList(glistp++, _alienWall_material);
    // gSPDisplayList(glistp++, _level_debug_geo_3_tri);
    // gSPDisplayList(glistp++, _alienUnderhang_material);
    // gSPDisplayList(glistp++, _level_debug_geo_4_tri);

    gDPFullSync(glistp++);
    gSPEndDisplayList(glistp++);

    osWritebackDCache(&i->dp, (s32)glistp - (s32)&i->dp);
    osWritebackDCache(&dynamicp, sizeof(Dynamic));

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