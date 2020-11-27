#include <ultra64.h>
#include "theme.h"
#include "src/defs.h"

Gfx _cutscene_theme_clear_screen[] = {
	gsSPEndDisplayList(),
};

struct LevelThemeGraphics gCutsceneTheme = {
    0,
    0,
    0,
    0,
    0,
    _cutscene_theme_clear_screen,
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    },
    0,
};