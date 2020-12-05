
#ifndef _GRAPHICS_LEVEL_THEME_GRAPHICS_H
#define _GRAPHICS_LEVEL_THEME_GRAPHICS_H

#include <ultra64.h>

enum LevelThemeMeshes {
	LevelThemeMeshesLSwitchUp,
	LevelThemeMeshesLSwitchDown,
	LevelThemeMeshesSSwitchUp,
	LevelThemeMeshesSSwitchDown,
	LevelThemeMeshesDoor,
	LevelThemeMeshesBreakPlatform,
	LevelThemeMeshesBreakBarrier,
	LevelThemeMeshesBreakFragment,
	LevelThemeMeshesMovingPlatform,
	LevelThemeMeshesEntranceExit,
	LevelThemeMeshesCount,
};


enum DynamicMaterialType {
    DynamicMaterialTypeSwitch,
    DynamicMaterialTypeDoor,
    DynamicMaterialTypeBreakable,
	DynamicMaterialTypeMovingPlatform,
	DynamicMaterialTypeEntranceExit,
	DynamicMaterialTypeEntranceExitOff,
};

enum TransparentMaterialType {
	TransparentMaterialTypeShadow,
	TransparentMaterialTypeGem,
	TransparentMaterialTypeShockwave,
	TransparentMaterialTypeSmoke,
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

typedef void (*AnimCallback)(void* themeBufferStart);

struct LevelThemeGraphics {
    Gfx** materials;
    int materialCount;
    Gfx** dynamicMaterials;
    Gfx** dynamicMaterialCleanup;
    int dynamicMaterialCount;
	Gfx* clear;
	Gfx* clearPal;
	Gfx* themeMeshes[LevelThemeMeshesCount];
	AnimCallback animCallback;
};

#endif