
#ifndef _GRAPHICS_LEVEL_THEME_GRAPHICS_H
#define _GRAPHICS_LEVEL_THEME_GRAPHICS_H

#include <ultra64.h>

enum LevelThemeMaterialType {
	LevelThemeMaterialTypeLowerFloor = 0,
	LevelThemeMaterialTypeUpperFloor = 1,
	LevelThemeMaterialTypeBarrierTop = 2,
	LevelThemeMaterialTypeWall       = 3,
	LevelThemeMaterialTypeUnderhang  = 4,
	LevelThemeMaterialTypeLava       = 5,
	LevelThemeMaterialTypeTrack      = 6,
};

struct LevelThemeGraphics {
    Gfx** materials;
    int materialCount;
    u8 clearColorR, clearColorG, clearColorB;
};

#endif