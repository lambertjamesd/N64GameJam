#include <ultra64.h>
#include "theme.h"
#include "src/defs.h"

Gfx _menu_theme_clear_screen[] = {
    gsDPSetFillColor(GPACK_RGBA5551(0, 0, 0, 1) << 16 | 
                    GPACK_RGBA5551(0, 0, 0, 1)),
    gsDPFillRectangle(0, 0, SCREEN_WD-1, SCREEN_HT-1),
	gsSPEndDisplayList(),
};

struct LevelThemeGraphics gMenuTheme = {
    0,
    0,
    0,
    0,
    0,
    _menu_theme_clear_screen,
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    },
    0,
};