
#include <ultra64.h>
#include "src/graphics/graphics.h"

Gfx rdpstateinit_dl_common[] = {
    gsDPSetEnvColor(0,0,0,0),
    gsDPSetPrimColor(0,0,0,0,0,0),
    gsDPSetBlendColor(0,0,0,0),
    gsDPSetFogColor(0,0,0,0),
    gsDPSetFillColor(0),
    gsDPSetPrimDepth(0,0),
    gsDPSetConvert(0,0,0,0,0,0),
    gsDPSetKeyR(0,0,0),
    gsDPSetKeyGB(0,0,0,0,0,0),

    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),

    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0),

    gsDPSetTileSize(0, 0, 0, (1 - 1) << G_TEXTURE_IMAGE_FRAC, (1 - 1) <<
		G_TEXTURE_IMAGE_FRAC),
    gsDPSetTileSize(1, 0, 0, (1 - 1) << G_TEXTURE_IMAGE_FRAC, (1 - 1) <<
		G_TEXTURE_IMAGE_FRAC),
    gsDPSetTileSize(2, 0, 0, (1 - 1) << G_TEXTURE_IMAGE_FRAC, (1 - 1) <<
		G_TEXTURE_IMAGE_FRAC),
    gsDPSetTileSize(3, 0, 0, (1 - 1) << G_TEXTURE_IMAGE_FRAC, (1 - 1) <<
		G_TEXTURE_IMAGE_FRAC),
    gsDPSetTileSize(4, 0, 0, (1 - 1) << G_TEXTURE_IMAGE_FRAC, (1 - 1) <<
		G_TEXTURE_IMAGE_FRAC),
    gsDPSetTileSize(5, 0, 0, (1 - 1) << G_TEXTURE_IMAGE_FRAC, (1 - 1) <<
		G_TEXTURE_IMAGE_FRAC),
    gsDPSetTileSize(6, 0, 0, (1 - 1) << G_TEXTURE_IMAGE_FRAC, (1 - 1) <<
		G_TEXTURE_IMAGE_FRAC),
    gsDPSetTileSize(7, 0, 0, (1 - 1) << G_TEXTURE_IMAGE_FRAC, (1 - 1) <<
		G_TEXTURE_IMAGE_FRAC),

		gsDPSetColorDither(G_CD_BAYER),

    gsDPPipeSync(),
    gsSPEndDisplayList(),
};

Gfx rdpstateinit_dl_pal[] = {
    gsDPSetScissor(G_SC_NON_INTERLACE, 0, 0, SCREEN_WD, SCREEN_HT_PAL),
    gsSPBranchList(rdpstateinit_dl_common),
};

Gfx rdpstateinit_dl[] = {
    gsDPSetScissor(G_SC_NON_INTERLACE, 0, 0, SCREEN_WD, SCREEN_HT_NTSC),
    gsSPBranchList(rdpstateinit_dl_common),
};

/* intialize the RSP state: */
Gfx setup_rspstate[] = {
    // gsSPViewport(&vp),
    gsSPClearGeometryMode((G_ZBUFFER | G_SHADE | G_SHADING_SMOOTH | 
			   G_CULL_BOTH | G_FOG | G_LIGHTING)),
    gsSPSetGeometryMode(G_ZBUFFER | G_SHADE | G_SHADING_SMOOTH | G_CULL_BACK),
    gsSPTexture(0, 0, 0, 0, G_OFF),
    gsSPEndDisplayList(),
};


