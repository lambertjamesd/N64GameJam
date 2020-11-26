
#include "buttons.h"

#include "tex.inc.c"


struct FontCharacter gButtonFontCharacters[] = {
    {ButtonFontMappingA, 0, 0, 9},
    {ButtonFontMappingB, 10, 0, 9},

    {ButtonFontMappingC_U, 20, 0, 9},
    {ButtonFontMappingC_R, 30, 0, 9},
    {ButtonFontMappingC_L, 40, 0, 9},
    {ButtonFontMappingC_D, 50, 0, 9},

    {ButtonFontMappingZ, 60, 0, 9},

    {ButtonFontMappingJoystick, 70, 0, 9},
    {ButtonFontMappingGemGone, 80, 0, 9, 3},
    {ButtonFontMappingGem, 90, 0, 9, 3},

    {ButtonFontMappingStart, 100, 0, 9},
    {ButtonFontMappingR, 110, 0, 9},
    {0, 0, 0, 0},
};

struct Font gButtonFont;

Gfx gButtonFontUse[] = {
	gsDPPipeSync(),
    gsSPClearGeometryMode(G_ZBUFFER),
    gsDPSetRenderMode(G_RM_XLU_SURF, G_RM_XLU_SURF2),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
    gsDPSetTextureFilter(G_TF_POINT),
    gsDPSetCycleType(G_CYC_1CYCLE),
    gsDPSetTexturePersp(G_TP_NONE),
    gsDPSetTextureLUT(G_TT_NONE),
	gsSPTexture(65535, 65535, 0, 0, 1),
	gsDPTileSync(),
	gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 120, _buttons_font_rgba16),
	gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 30, 0, 7, 0, G_TX_WRAP | G_TX_NOMIRROR, 4, 0, G_TX_WRAP | G_TX_NOMIRROR, 7, 0),
	gsDPLoadSync(),
	gsDPLoadTile(7, 0, 0, 476, 44),
	gsDPPipeSync(),
	gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 30, 0, 0, 0, G_TX_WRAP | G_TX_NOMIRROR, 4, 0, G_TX_WRAP | G_TX_NOMIRROR, 7, 0),
	gsDPSetTileSize(0, 0, 0, 476, 44),
	gsSPEndDisplayList(),
};