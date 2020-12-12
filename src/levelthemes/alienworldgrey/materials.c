#include <ultra64.h>

#include "tex/tex_floor.inc.c"
#include "tex/tex_wall.inc.c"
#include "tex/tex_underhang.inc.c"
#include "tex/tex_swamp.inc.c"
#include "tex/tex_move_platform.inc.c"
#include "geo/switches.inc.c"
#include "geo/door.inc.c"
#include "geo/breakable.inc.c"
#include "geo/movingplatform.inc.c"
#include "geo/rocksmall.inc.c"
#include "geo/rockbig.inc.c"
#include "geo/rockfragment.inc.c"
#include "geo/entranceexit.inc.c"
#include "src/defs.h"

Gfx _aliengreyFloor_material[] = {
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_1CYCLE),
    gsDPSetRenderMode(G_RM_ZB_OPA_SURF, G_RM_ZB_OPA_SURF2),
    gsSPSetGeometryMode(G_SHADE | G_SHADING_SMOOTH),
	gsDPSetCombineLERP(TEXEL0, 0, SHADE, 0, 0, 0, 0, 1, TEXEL0, 0, SHADE, 0, 0, 0, 0, 1),
	gsDPPipeSync(),
	gsSPTexture(32535, 32535, 0, 0, 1),
	gsDPTileSync(),
	gsDPSetTextureImage(G_IM_FMT_I, G_IM_SIZ_8b, 32, _aliengreyFloorLum_i4),
	gsDPSetTile(G_IM_FMT_I, G_IM_SIZ_8b, 4, 0, 7, 0, G_TX_WRAP | G_TX_NOMIRROR, 6, 0, G_TX_WRAP | G_TX_NOMIRROR, 6, 0),
	gsDPLoadSync(),
	gsDPLoadTile(7, 0, 0, 126, 252),
	gsDPPipeSync(),
	gsDPSetTile(G_IM_FMT_I, G_IM_SIZ_4b, 4, 0, 0, 0, G_TX_WRAP | G_TX_NOMIRROR, 6, 0, G_TX_WRAP | G_TX_NOMIRROR, 6, 0),
	gsDPSetTileSize(0, 0, 0, 252, 252),
	gsSPEndDisplayList(),
};

Gfx _aliengreyWall_material[] = {
	gsDPPipeSync(),
	gsSPTexture(65535, 65535, 0, 0, 1),
	gsDPTileSync(),
	gsDPSetTextureImage(G_IM_FMT_I, G_IM_SIZ_8b, 16, _aliengreyWallLum_i4),
	gsDPSetTile(G_IM_FMT_I, G_IM_SIZ_8b, 2, 0, 7, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, 0),
	gsDPLoadSync(),
	gsDPLoadTile(7, 0, 0, 62, 124),
	gsDPPipeSync(),
	gsDPSetTile(G_IM_FMT_I, G_IM_SIZ_4b, 2, 0, 0, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, 0),
	gsDPSetTileSize(0, 0, 0, 124, 124),
	gsSPEndDisplayList(),
};

Gfx _aliengreyUnderhang_material[] = {
	gsDPPipeSync(),
	gsDPSetRenderMode(G_RM_ZB_XLU_SURF, G_RM_ZB_XLU_SURF2),
	gsDPSetCombineLERP(TEXEL0, 0, SHADE, 0, 0, 0, 0, TEXEL0, TEXEL0, 0, SHADE, 0, 0, 0, 0, TEXEL0),
	gsSPClearGeometryMode(G_CULL_BACK),
	gsSPTexture(65535, 65535, 0, 0, 1),
	gsDPSetTextureLUT(G_TT_RGBA16),
	gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, _aliengreyUnderhang_ci4_pal_rgba16),
	gsDPTileSync(),
	gsDPSetTile(0, 0, 0, 256, 7, 0, G_TX_WRAP | G_TX_NOMIRROR, 0, 0, G_TX_WRAP | G_TX_NOMIRROR, 0, 0),
	gsDPLoadSync(),
	gsDPLoadTLUTCmd(7, 4),
	gsDPPipeSync(),
	gsDPTileSync(),
	gsDPSetTextureImage(G_IM_FMT_CI, G_IM_SIZ_8b, 16, _aliengreyUnderhang_ci4),
	gsDPSetTile(G_IM_FMT_CI, G_IM_SIZ_8b, 2, 0, 7, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, 0),
	gsDPLoadSync(),
	gsDPLoadTile(7, 0, 0, 62, 124),
	gsDPPipeSync(),
	gsDPSetTile(G_IM_FMT_CI, G_IM_SIZ_4b, 2, 0, 0, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, 0),
	gsDPSetTileSize(0, 0, 0, 124, 124),
	gsDPSetPrimColor(0, 0, 187, 187, 187, 255),
	gsSPEndDisplayList(),
};

Gfx _aliengrey_swamp_material[] = {
	gsDPPipeSync(),
	gsDPSetRenderMode(G_RM_ZB_OPA_SURF, G_RM_ZB_OPA_SURF2),
	gsDPSetEnvColor(255, 255, 255, 255),
	gsDPSetCombineLERP(TEXEL0, 0, ENVIRONMENT, 0, 0, 0, 0, ENVIRONMENT, TEXEL0, 0, ENVIRONMENT, 0, 0, 0, 0, ENVIRONMENT),
	gsSPSetGeometryMode(G_CULL_BACK),
	gsSPTexture(65535, 65535, 0, 0, 1),
	gsDPSetTextureLUT(G_TT_RGBA16),
	gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, _aliengrey_swamp_tex_ci4_pal_rgba16),
	gsDPTileSync(),
	gsDPSetTile(0, 0, 0, 256, 7, 0, G_TX_WRAP | G_TX_NOMIRROR, 0, 0, G_TX_WRAP | G_TX_NOMIRROR, 0, 0),
	gsDPLoadSync(),
	gsDPLoadTLUTCmd(7, 6),
	gsDPPipeSync(),
	gsDPTileSync(),
	gsDPSetTextureImage(G_IM_FMT_CI, G_IM_SIZ_8b, 16, _aliengrey_swamp_tex_ci4),
	gsDPSetTile(G_IM_FMT_CI, G_IM_SIZ_8b, 2, 0, 7, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, 0),
	gsDPLoadSync(),
	gsDPLoadTile(7, 0, 0, 62, 124),
	gsDPPipeSync(),
	gsDPSetTile(G_IM_FMT_CI, G_IM_SIZ_4b, 2, 0, 0, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, 0),
	gsSPEndDisplayList(),
};

Gfx _aliengrey_swamp_material_anim[] = {
	gsSPDisplayList(_aliengrey_swamp_material),
	gsDPSetTileSize(0, 0, 0, 124, 124),
	gsSPEndDisplayList(),
};

Gfx _aliengrey_track_material_anim[] = {
    gsDPSetPrimColor(0, 0, 64, 64, 64, 255),
	gsDPSetCombineLERP(0, 0, 0, PRIMITIVE, 0, 0, 0, ENVIRONMENT, 0, 0, 0, PRIMITIVE, 0, 0, 0, ENVIRONMENT),
	gsSPEndDisplayList(),
};

u8 __attribute__((aligned(8))) _aliengrey_background_gradient[] = {
	0x1, 0x1, 0x1, 0x1, 0x12, 0x11, 0x11, 0x11, 0x1, 
	0x1, 0x1, 0x1, 0x21, 0x23, 0x22, 0x22, 0x31, 0x32, 
	0x32, 0x31, 0x3, 0x3, 0x3, 0x3, 0x32, 0x32, 0x32, 
	0x32, 0x0, 0x3, 0x0, 0x3, 0x33, 0x32, 0x33, 0x32, 
	0x23, 0x23, 0x23, 0x23, 0x32, 0x33, 0x32, 0x33, 0x23, 
	0x23, 0x23, 0x23, 0x33, 0x33, 0x33, 0x33, 0x23, 0x23, 
	0x23, 0x23, 0x32, 0x33, 0x32, 0x33, 0x0, 0x4, 0x0, 
	0x4, 0x40, 0x40, 0x40, 0x40, 0x4, 0x4, 0x4, 0x4, 
	0x40, 0x44, 0x40, 0x40, 0x4, 0x4, 0x4, 0x4, 0x43, 
	0x33, 0x34, 0x33, 0x43, 0x43, 0x43, 0x43, 0x34, 0x34, 
	0x34, 0x33, 0x43, 0x43, 0x43, 0x43, 0x33, 0x34, 0x34, 
	0x34, 0x45, 0x44, 0x45, 0x44, 0x54, 0x54, 0x54, 0x54, 
	0x44, 0x45, 0x44, 0x45, 0x54, 0x54, 0x54, 0x54, 0x45, 
	0x45, 0x45, 0x45, 0x54, 0x54, 0x54, 0x55, 0x45, 0x45, 
	0x45, 0x45, 0x55, 0x55, 0x55, 0x55, 0x65, 0x65, 0x65, 
	0x65, 0x56, 0x55, 0x56, 0x56, 0x65, 0x65, 0x65, 0x65, 
	0x55, 0x56, 0x55, 0x56, 0x55, 0x75, 0x55, 0x75, 0x55, 
	0x55, 0x56, 0x56, 0x75, 0x75, 0x75, 0x75, 0x55, 0x55, 
	0x55, 0x55, 0x75, 0x75, 0x75, 0x75, 0x57, 0x55, 0x57, 
	0x57, 0x75, 0x75, 0x75, 0x75, 0x57, 0x57, 0x57, 0x57, 
	0x75, 0x77, 0x75, 0x75, 0x57, 0x57, 0x57, 0x57, 0x77, 
	0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x87, 0x87, 
	0x87, 0x87, 0x78, 0x98, 0x77, 0x77, 0x88, 0x87, 0x88, 
	0x87, 0x79, 0x98, 0x98, 0x98, 0x89, 0x89, 0x89, 0x89, 
	0x98, 0x98, 0x98, 0x98, 0x88, 0x89, 0x88, 0x89, 0x88, 
	0x88, 0x88, 0x88, 0x8A, 0x8A, 0x8A, 0x8A, 0xA8, 0xA8, 
	0xA8, 0xA8, 0x8A, 0x8A, 0x8A, 0x8A, 0xAA, 0xA8, 0xAA, 
	0xAA, 0x8A, 0x8A, 0x8A, 0x8A, 0xA8, 0xAA, 0xA8, 0xAA, 
	0x8A, 0x8A, 0x8A, 0x8A, 
};

u8 __attribute__((aligned(8))) _aliengrey_background_gradient_pal[32] = {
	0x3A, 0x23, 0x42, 0x63, 0x42, 0x23, 0x3A, 0x21, 0x3A, 
	0x1D, 0x29, 0xDD, 0x29, 0xDB, 0x29, 0x5B, 0x21, 0x19, 
	0x21, 0x1B, 0x21, 0x15, 
};

Gfx _aliengrey_clearScreen[] = {
    gsDPPipeSync(),
	gsDPSetCycleType(G_CYC_1CYCLE),
	gsDPSetTexturePersp(G_TP_NONE),
	gsDPSetCombineLERP(0, 0, 0, TEXEL0, 0, 0, 0, ENVIRONMENT, 0, 0, 0, TEXEL0, 0, 0, 0, ENVIRONMENT),
	gsSPTexture(65535, 65535, 0, 0, 1),
	gsDPSetTextureLUT(G_TT_RGBA16),
	gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, _aliengrey_background_gradient_pal),
	gsDPTileSync(),
	gsDPSetTile(0, 0, 0, 256, 7, 0, G_TX_WRAP | G_TX_NOMIRROR, 0, 0, G_TX_WRAP | G_TX_NOMIRROR, 0, 0),
	gsDPLoadSync(),
	gsDPLoadTLUTCmd(7, 10),
	gsDPPipeSync(),
	gsDPTileSync(),
	gsDPSetTextureImage(G_IM_FMT_CI, G_IM_SIZ_8b, 4, _aliengrey_background_gradient),
	gsDPSetTile(G_IM_FMT_CI, G_IM_SIZ_8b, 1, 0, 7, 0, G_TX_WRAP | G_TX_NOMIRROR, 6, 0, G_TX_WRAP | G_TX_MIRROR, 3, 0),
	gsDPLoadSync(),
	gsDPLoadTile(7, 0, 0, 14, 252),
	gsDPPipeSync(),
	gsDPSetTile(G_IM_FMT_CI, G_IM_SIZ_4b, 1, 0, 0, 0, G_TX_WRAP | G_TX_NOMIRROR, 6, 0, G_TX_WRAP | G_TX_MIRROR, 3, 0),
	gsDPSetTileSize(0, 0, 0, 28, 252),
	gsSPTextureRectangle(
		0, 0, 
		SCREEN_WD << 2, SCREEN_HT_NTSC << 2, 
		G_TX_RENDERTILE, 
		0, 0,
		0x400, 0x400 * 64 / SCREEN_HT_NTSC
	),
	gsDPPipeSync(),
	gsDPSetTexturePersp(G_TP_PERSP),
	gsSPEndDisplayList(),
};

Gfx _aliengrey_clearScreenPal[] = {
    gsDPPipeSync(),
	gsDPSetCycleType(G_CYC_1CYCLE),
	gsDPSetTexturePersp(G_TP_NONE),
	gsDPSetCombineLERP(0, 0, 0, TEXEL0, 0, 0, 0, ENVIRONMENT, 0, 0, 0, TEXEL0, 0, 0, 0, ENVIRONMENT),
	gsSPTexture(65535, 65535, 0, 0, 1),
	gsDPSetTextureLUT(G_TT_RGBA16),
	gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, _aliengrey_background_gradient_pal),
	gsDPTileSync(),
	gsDPSetTile(0, 0, 0, 256, 7, 0, G_TX_WRAP | G_TX_NOMIRROR, 0, 0, G_TX_WRAP | G_TX_NOMIRROR, 0, 0),
	gsDPLoadSync(),
	gsDPLoadTLUTCmd(7, 10),
	gsDPPipeSync(),
	gsDPTileSync(),
	gsDPSetTextureImage(G_IM_FMT_CI, G_IM_SIZ_8b, 4, _aliengrey_background_gradient),
	gsDPSetTile(G_IM_FMT_CI, G_IM_SIZ_8b, 1, 0, 7, 0, G_TX_WRAP | G_TX_NOMIRROR, 6, 0, G_TX_WRAP | G_TX_MIRROR, 3, 0),
	gsDPLoadSync(),
	gsDPLoadTile(7, 0, 0, 14, 252),
	gsDPPipeSync(),
	gsDPSetTile(G_IM_FMT_CI, G_IM_SIZ_4b, 1, 0, 0, 0, G_TX_WRAP | G_TX_NOMIRROR, 6, 0, G_TX_WRAP | G_TX_MIRROR, 3, 0),
	gsDPSetTileSize(0, 0, 0, 28, 252),
	gsSPTextureRectangle(
		0, 0, 
		SCREEN_WD << 2, SCREEN_HT_PAL << 2, 
		G_TX_RENDERTILE, 
		0, 0,
		0x400, 0x400 * 64 / SCREEN_HT_PAL
	),
	gsDPPipeSync(),
	gsDPSetTexturePersp(G_TP_PERSP),
	gsSPEndDisplayList(),
};
