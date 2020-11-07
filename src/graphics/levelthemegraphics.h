
#ifndef _GRAPHICS_LEVEL_THEME_GRAPHICS_H
#define _GRAPHICS_LEVEL_THEME_GRAPHICS_H

#include <ultra64.h>

enum LevelThemeMeshes {
	LevelThemeMeshesLSwitchUp,
	LevelThemeMeshesLSwitchDown,
	LevelThemeMeshesSSwitchUp,
	LevelThemeMeshesSSwitchDown,
	LevelThemeMeshesDoor,
	LevelThemeMeshesCount,
};

#define SWITCH_COLOR_COUNT	5

extern u32 gSwitchColors[];
extern u32 gSwitchDarkColors[];

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
    Gfx** dynamicMaterials;
    int dynamicMaterialCount;
	Gfx* clear;
	Gfx* themeMeshes[LevelThemeMeshesCount];
};

#endif