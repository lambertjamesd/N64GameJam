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

Gfx _alienFloor_material[] = {
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_1CYCLE),
    gsDPSetRenderMode(G_RM_ZB_OPA_SURF, G_RM_ZB_OPA_SURF2),
    gsSPSetGeometryMode(G_SHADE | G_SHADING_SMOOTH),
	gsDPSetCombineLERP(TEXEL0, 0, SHADE, 0, 0, 0, 0, 1, TEXEL0, 0, SHADE, 0, 0, 0, 0, 1),
	gsDPPipeSync(),
	gsSPTexture(32535, 32535, 0, 0, 1),
	gsDPTileSync(),
	gsDPSetTextureImage(G_IM_FMT_I, G_IM_SIZ_8b, 32, _alienFloorLum_i4),
	gsDPSetTile(G_IM_FMT_I, G_IM_SIZ_8b, 4, 0, 7, 0, G_TX_WRAP | G_TX_NOMIRROR, 6, 0, G_TX_WRAP | G_TX_NOMIRROR, 6, 0),
	gsDPLoadSync(),
	gsDPLoadTile(7, 0, 0, 126, 252),
	gsDPPipeSync(),
	gsDPSetTile(G_IM_FMT_I, G_IM_SIZ_4b, 4, 0, 0, 0, G_TX_WRAP | G_TX_NOMIRROR, 6, 0, G_TX_WRAP | G_TX_NOMIRROR, 6, 0),
	gsDPSetTileSize(0, 0, 0, 252, 252),
	gsSPEndDisplayList(),
};

Gfx _alienWall_material[] = {
	gsDPPipeSync(),
	gsSPTexture(65535, 65535, 0, 0, 1),
	gsDPTileSync(),
	gsDPSetTextureImage(G_IM_FMT_I, G_IM_SIZ_8b, 16, _alienWallLum_i4),
	gsDPSetTile(G_IM_FMT_I, G_IM_SIZ_8b, 2, 0, 7, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, 0),
	gsDPLoadSync(),
	gsDPLoadTile(7, 0, 0, 62, 124),
	gsDPPipeSync(),
	gsDPSetTile(G_IM_FMT_I, G_IM_SIZ_4b, 2, 0, 0, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, 0),
	gsDPSetTileSize(0, 0, 0, 124, 124),
	gsSPEndDisplayList(),
};

Gfx _alienUnderhang_material[] = {
	gsDPPipeSync(),
	gsDPSetRenderMode(G_RM_ZB_XLU_SURF, G_RM_ZB_XLU_SURF2),
	gsDPSetCombineLERP(TEXEL0, 0, SHADE, 0, 0, 0, 0, TEXEL0, TEXEL0, 0, SHADE, 0, 0, 0, 0, TEXEL0),
	gsSPClearGeometryMode(G_CULL_BACK),
	gsSPTexture(65535, 65535, 0, 0, 1),
	gsDPSetTextureLUT(G_TT_RGBA16),
	gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, _alienUnderhang_ci4_pal_rgba16),
	gsDPTileSync(),
	gsDPSetTile(0, 0, 0, 256, 7, 0, G_TX_WRAP | G_TX_NOMIRROR, 0, 0, G_TX_WRAP | G_TX_NOMIRROR, 0, 0),
	gsDPLoadSync(),
	gsDPLoadTLUTCmd(7, 4),
	gsDPPipeSync(),
	gsDPTileSync(),
	gsDPSetTextureImage(G_IM_FMT_CI, G_IM_SIZ_8b, 16, _alienUnderhang_ci4),
	gsDPSetTile(G_IM_FMT_CI, G_IM_SIZ_8b, 2, 0, 7, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, 0),
	gsDPLoadSync(),
	gsDPLoadTile(7, 0, 0, 62, 124),
	gsDPPipeSync(),
	gsDPSetTile(G_IM_FMT_CI, G_IM_SIZ_4b, 2, 0, 0, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, 0),
	gsDPSetTileSize(0, 0, 0, 124, 124),
	gsDPSetPrimColor(0, 0, 187, 187, 187, 255),
	gsSPEndDisplayList(),
};

Gfx _alien_swamp_material[] = {
	gsDPPipeSync(),
	gsDPSetRenderMode(G_RM_ZB_OPA_SURF, G_RM_ZB_OPA_SURF2),
	gsDPSetEnvColor(255, 255, 255, 255),
	gsDPSetCombineLERP(TEXEL0, 0, ENVIRONMENT, 0, 0, 0, 0, ENVIRONMENT, TEXEL0, 0, ENVIRONMENT, 0, 0, 0, 0, ENVIRONMENT),
	gsSPSetGeometryMode(G_CULL_BACK),
	gsSPTexture(65535, 65535, 0, 0, 1),
	gsDPSetTextureLUT(G_TT_RGBA16),
	gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, _alien_swamp_tex_ci4_pal_rgba16),
	gsDPTileSync(),
	gsDPSetTile(0, 0, 0, 256, 7, 0, G_TX_WRAP | G_TX_NOMIRROR, 0, 0, G_TX_WRAP | G_TX_NOMIRROR, 0, 0),
	gsDPLoadSync(),
	gsDPLoadTLUTCmd(7, 4),
	gsDPPipeSync(),
	gsDPTileSync(),
	gsDPSetTextureImage(G_IM_FMT_CI, G_IM_SIZ_8b, 16, _alien_swamp_tex_ci4),
	gsDPSetTile(G_IM_FMT_CI, G_IM_SIZ_8b, 2, 0, 7, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, 0),
	gsDPLoadSync(),
	gsDPLoadTile(7, 0, 0, 62, 124),
	gsDPPipeSync(),
	gsDPSetTile(G_IM_FMT_CI, G_IM_SIZ_4b, 2, 0, 0, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, 0),
	gsSPEndDisplayList(),
};

Gfx _alien_swamp_material_anim[] = {
	gsSPDisplayList(_alien_swamp_material),
	gsDPSetTileSize(0, 0, 0, 124, 124),
	gsSPEndDisplayList(),
};

Gfx _alien_track_material_anim[] = {
    gsDPSetPrimColor(0, 0, 64, 64, 64, 255),
	gsDPSetCombineLERP(0, 0, 0, PRIMITIVE, 0, 0, 0, ENVIRONMENT, 0, 0, 0, PRIMITIVE, 0, 0, 0, ENVIRONMENT),
	gsSPEndDisplayList(),
};

u8 __attribute__((aligned(8))) _alien_background_gradient[] = {
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
	0x0, 0x0, 0x0, 0x0, 0x0, 0x10, 0x10, 0x10, 0x10, 
	0x1, 0x0, 0x1, 0x0, 0x10, 0x10, 0x10, 0x10, 0x0, 
	0x1, 0x0, 0x1, 0x10, 0x10, 0x10, 0x10, 0x1, 0x1, 
	0x1, 0x1, 0x10, 0x10, 0x10, 0x10, 0x1, 0x1, 0x1, 
	0x1, 0x21, 0x21, 0x21, 0x21, 0x12, 0x11, 0x11, 0x11, 
	0x21, 0x21, 0x21, 0x21, 0x11, 0x12, 0x11, 0x11, 0x21, 
	0x21, 0x21, 0x21, 0x11, 0x11, 0x11, 0x11, 0x31, 0x31, 
	0x31, 0x31, 0x11, 0x13, 0x11, 0x13, 0x31, 0x31, 0x31, 
	0x31, 0x13, 0x13, 0x13, 0x13, 0x31, 0x31, 0x31, 0x31, 
	0x13, 0x13, 0x13, 0x13, 0x33, 0x31, 0x33, 0x31, 0x13, 
	0x13, 0x13, 0x13, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 
	0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 
	0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 
	0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 
	0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 
	0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x43, 0x33, 
	0x33, 0x33, 0x33, 0x33, 0x33, 0x53, 0x53, 0x53, 0x53, 
	0x35, 0x33, 0x35, 0x33, 0x63, 0x63, 0x63, 0x63, 0x33, 
	0x36, 0x36, 0x36, 0x66, 0x63, 0x63, 0x63, 0x36, 0x36, 
	0x36, 0x36, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 
	0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 
	0x66, 0x66, 0x66, 0x66, 0x66, 0x67, 0x66, 0x67, 0x76, 
	0x76, 0x76, 0x76, 0x67, 0x67, 0x67, 0x67, 0x77, 0x77, 
	0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 
	0x77, 0x77, 0x77, 0x77, 0x77, 0x87, 0x87, 0x87, 0x87, 
	0x77, 0x78, 0x77, 0x78, 
};

u8 __attribute__((aligned(8))) _alien_background_gradient_pal[] = {
	0x40, 0x15, 0x30, 0x15, 0x28, 0x13, 0x20, 0x13, 0x10, 
	0x13, 0x10, 0x11, 0x8, 0x11, 0x0, 0x11, 0x0, 0xD, 
};

Gfx _alien_clearScreen[] = {
    gsDPPipeSync(),
	gsDPSetCycleType(G_CYC_1CYCLE),
	gsDPSetTexturePersp(G_TP_NONE),
	gsDPSetCombineLERP(0, 0, 0, TEXEL0, 0, 0, 0, ENVIRONMENT, 0, 0, 0, TEXEL0, 0, 0, 0, ENVIRONMENT),
	gsSPTexture(65535, 65535, 0, 0, 1),
	gsDPSetTextureLUT(G_TT_RGBA16),
	gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, _alien_background_gradient_pal),
	gsDPTileSync(),
	gsDPSetTile(0, 0, 0, 256, 7, 0, G_TX_WRAP | G_TX_NOMIRROR, 0, 0, G_TX_WRAP | G_TX_NOMIRROR, 0, 0),
	gsDPLoadSync(),
	gsDPLoadTLUTCmd(7, 8),
	gsDPPipeSync(),
	gsDPTileSync(),
	gsDPSetTextureImage(G_IM_FMT_CI, G_IM_SIZ_8b, 4, _alien_background_gradient),
	gsDPSetTile(G_IM_FMT_CI, G_IM_SIZ_8b, 1, 0, 7, 0, G_TX_WRAP | G_TX_NOMIRROR, 6, 0, G_TX_WRAP | G_TX_NOMIRROR, 3, 0),
	gsDPLoadSync(),
	gsDPLoadTile(7, 0, 0, 14, 252),
	gsDPPipeSync(),
	gsDPSetTile(G_IM_FMT_CI, G_IM_SIZ_4b, 1, 0, 0, 0, G_TX_WRAP | G_TX_NOMIRROR, 6, 0, G_TX_WRAP | G_TX_NOMIRROR, 3, 0),
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

Gfx _alien_clearScreenPal[] = {
    gsDPPipeSync(),
	gsDPSetCycleType(G_CYC_1CYCLE),
	gsDPSetTexturePersp(G_TP_NONE),
	gsDPSetCombineLERP(0, 0, 0, TEXEL0, 0, 0, 0, ENVIRONMENT, 0, 0, 0, TEXEL0, 0, 0, 0, ENVIRONMENT),
	gsSPTexture(65535, 65535, 0, 0, 1),
	gsDPSetTextureLUT(G_TT_RGBA16),
	gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, _alien_background_gradient_pal),
	gsDPTileSync(),
	gsDPSetTile(0, 0, 0, 256, 7, 0, G_TX_WRAP | G_TX_NOMIRROR, 0, 0, G_TX_WRAP | G_TX_NOMIRROR, 0, 0),
	gsDPLoadSync(),
	gsDPLoadTLUTCmd(7, 8),
	gsDPPipeSync(),
	gsDPTileSync(),
	gsDPSetTextureImage(G_IM_FMT_CI, G_IM_SIZ_8b, 4, _alien_background_gradient),
	gsDPSetTile(G_IM_FMT_CI, G_IM_SIZ_8b, 1, 0, 7, 0, G_TX_WRAP | G_TX_NOMIRROR, 6, 0, G_TX_WRAP | G_TX_NOMIRROR, 3, 0),
	gsDPLoadSync(),
	gsDPLoadTile(7, 0, 0, 14, 252),
	gsDPPipeSync(),
	gsDPSetTile(G_IM_FMT_CI, G_IM_SIZ_4b, 1, 0, 0, 0, G_TX_WRAP | G_TX_NOMIRROR, 6, 0, G_TX_WRAP | G_TX_NOMIRROR, 3, 0),
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