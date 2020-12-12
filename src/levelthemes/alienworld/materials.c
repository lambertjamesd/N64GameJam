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
	0x1, 0x1, 0x1, 0x1, 0x10, 0x10, 0x10, 0x10, 0x20, 
	0x20, 0x20, 0x20, 0x0, 0x2, 0x0, 0x2, 0x20, 0x20, 
	0x20, 0x20, 0x2, 0x2, 0x2, 0x2, 0x21, 0x21, 0x21, 
	0x22, 0x12, 0x12, 0x12, 0x12, 0x22, 0x22, 0x22, 0x21, 
	0x23, 0x23, 0x23, 0x22, 0x32, 0x32, 0x32, 0x32, 0x3, 
	0x3, 0x3, 0x3, 0x32, 0x32, 0x32, 0x32, 0x23, 0x23, 
	0x23, 0x23, 0x30, 0x33, 0x33, 0x33, 0x3, 0x3, 0x3, 
	0x3, 0x33, 0x33, 0x33, 0x33, 0x33, 0x43, 0x33, 0x43, 
	0x33, 0x33, 0x33, 0x33, 0x43, 0x43, 0x43, 0x43, 0x53, 
	0x53, 0x53, 0x53, 0x35, 0x35, 0x35, 0x35, 0x55, 0x55, 
	0x55, 0x55, 0x45, 0x45, 0x45, 0x45, 0x55, 0x55, 0x55, 
	0x55, 0x55, 0x45, 0x45, 0x45, 0x55, 0x55, 0x54, 0x55, 
	0x45, 0x45, 0x45, 0x45, 0x55, 0x55, 0x55, 0x55, 0x45, 
	0x45, 0x45, 0x45, 0x54, 0x55, 0x54, 0x55, 0x45, 0x45, 
	0x45, 0x45, 0x55, 0x54, 0x55, 0x54, 0x64, 0x64, 0x64, 
	0x64, 0x46, 0x46, 0x46, 0x46, 0x64, 0x64, 0x64, 0x64, 
	0x46, 0x46, 0x46, 0x46, 0x64, 0x64, 0x64, 0x64, 0x47, 
	0x44, 0x47, 0x44, 0x64, 0x64, 0x64, 0x64, 0x57, 0x57, 
	0x57, 0x57, 0x44, 0x64, 0x44, 0x64, 0x47, 0x47, 0x47, 
	0x47, 0x74, 0x74, 0x74, 0x74, 0x47, 0x47, 0x47, 0x47, 
	0x74, 0x77, 0x74, 0x77, 0x48, 0x44, 0x48, 0x44, 0x99, 
	0x99, 0x99, 0x99, 0x79, 0x79, 0x79, 0x79, 0x9A, 0x9A, 
	0x9A, 0x9A, 0x89, 0x89, 0x89, 0x89, 0x98, 0x98, 0x99, 
	0x98, 0x89, 0x89, 0x88, 0x89, 0x98, 0x98, 0x98, 0x98, 
	0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x88, 
	0x88, 0x88, 0x8A, 0xA8, 0xA8, 0xA8, 0xA8, 0x88, 0x88, 
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 
	0x88, 0x88, 0x88, 0x88, 
};

u8 __attribute__((aligned(8))) _alien_background_gradient_pal[32] = {
	0x40, 0x55, 0x40, 0x59, 0x40, 0x15, 0x38, 0x15, 0x20, 
	0x13, 0x28, 0x15, 0x20, 0x15, 0x18, 0x13, 0x0, 0x11, 
	0x18, 0x11, 0x8, 0x11, 
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
	gsDPLoadTLUTCmd(7, 10),
	gsDPPipeSync(),
	gsDPTileSync(),
	gsDPSetTextureImage(G_IM_FMT_CI, G_IM_SIZ_8b, 4, _alien_background_gradient),
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
	gsDPLoadTLUTCmd(7, 10),
	gsDPPipeSync(),
	gsDPTileSync(),
	gsDPSetTextureImage(G_IM_FMT_CI, G_IM_SIZ_8b, 4, _alien_background_gradient),
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