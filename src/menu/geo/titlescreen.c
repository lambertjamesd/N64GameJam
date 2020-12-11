
#include <ultra64.h>

#include "titlescreen.inc.c"

#define NUM_BITMAPS     1

Bitmap titleScreenBitmaps[NUM_BITMAPS] = {{
    256,
    256,
    0, 0,
    _titleScreenRendered_tex,
    8,
    0,
}};

Gfx tileScreenGfx[NUM_DL(NUM_BITMAPS)] = {{1}};

Sprite titleScreenSprite = {
    0, 0,
    256,64,
    1.0f, 1.0f,
    0, 0,
    SP_TRANSPARENT,
    0x1234,
    255, 255, 255, 255,
    0, 0, NULL,
    0, 1,
    NUM_BITMAPS,
    NUM_DL(NUM_BITMAPS),
    8, 64,
    G_IM_FMT_RGBA,
    G_IM_SIZ_16b,
    titleScreenBitmaps,
    tileScreenGfx,
    tileScreenGfx,
};