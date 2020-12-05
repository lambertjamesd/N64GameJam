#include <ultra64.h>
#include "theme.h"
#include "src/defs.h"

Gfx _menu_theme_clear_screen[] = {
    gsDPSetFillColor(GPACK_RGBA5551(0, 0, 0, 1) << 16 | 
                    GPACK_RGBA5551(0, 0, 0, 1)),
    gsDPFillRectangle(0, 0, SCREEN_WD-1, SCREEN_HT_NTSC-1),
	gsSPEndDisplayList(),
};

Gfx _menu_theme_clear_screen_pal[] = {
    gsDPSetFillColor(GPACK_RGBA5551(0, 0, 0, 1) << 16 | 
                    GPACK_RGBA5551(0, 0, 0, 1)),
    gsDPFillRectangle(0, 0, SCREEN_WD-1, SCREEN_HT_PAL-1),
	gsSPEndDisplayList(),
};

struct LevelThemeGraphics gMenuTheme = {
    0,
    0,
    0,
    0,
    0,
    _menu_theme_clear_screen,
    _menu_theme_clear_screen_pal,
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    },
    0,
};