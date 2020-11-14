
#include "theme.h"
#include "header.h"
#include "src/time/time.h"

Gfx* _gAlienWorldMaterials[] = {
    _alienFloor_material,
    0,
    0,
    _alienWall_material,
    _alienUnderhang_material,
    _alien_swamp_material_anim,
    _alien_track_material_anim,
};

Gfx* _gAlienWorldDynamicMaterials[] = {
    _alien_switch_material,
    _alien_puzzle_door_material,
    _alien_breakable_material,
    _alien_moving_platform_material,
    _entrance_exit_material,
    _entrance_exit_off_material,
};

Gfx* _gAlienWorldDynamicMaterialCleanup[] = {
    0,
    0,
    0,
    0,
    _entrance_exit_material_revert,
    _entrance_exit_material_revert,
};

#define SCROLL_SPEED_X 0.3f
#define SCROLL_SPEED_Y 0.2f

static float gAlienScrollX = 0.0f;
static float gAlienScrollY = 0.0f;

void alienWorldAnimate(void* themeBufferStart) {
    u32 swampStart = ((u32)_alien_swamp_material_anim) & 0xFFFFFF;
    swampStart += (u32)themeBufferStart;

    Gfx* texDL = (Gfx*)swampStart;

    gAlienScrollX += SCROLL_SPEED_X * gTimeDelta;
    gAlienScrollY += SCROLL_SPEED_Y * gTimeDelta;

    if (gAlienScrollX > 1.0f) {
        gAlienScrollX -= 1.0f;
    }

    if (gAlienScrollY > 1.0f) {
        gAlienScrollY -= 1.0f;
    }

    int xOff = (int)(gAlienScrollX * 128.0f);
    int yOff = (int)(gAlienScrollY * 128.0f);

    gSPDisplayList(texDL++, _alien_swamp_material);
    gDPSetTileSize(texDL++, 0, xOff, yOff, xOff + 124, yOff + 124);
    gSPEndDisplayList(texDL++);

    osWritebackDCache((void*)swampStart, sizeof(Gfx) * 3);
}

struct LevelThemeGraphics gAlienWorldLevelTheme = {
    _gAlienWorldMaterials,
    sizeof(_gAlienWorldMaterials) / sizeof(*_gAlienWorldMaterials),
    _gAlienWorldDynamicMaterials,
    _gAlienWorldDynamicMaterialCleanup,
    sizeof(_gAlienWorldDynamicMaterials) / sizeof(*_gAlienWorldDynamicMaterials),
    _alien_clearScreen,
    {
        _alien_lswitch_up_tri, 
        _alien_lswitch_down_tri, 
        _alien_sswitch_up_tri, 
        _alien_sswitch_down_tri, 
        _alien_puzzle_door_mesh_tri_0,
        _alien_rock_small_tri,
        _alien_rock_big_tri,
        _alien_moving_platform_tri_0,
        _entrance_exit_mesh_tri_0,
    },
    alienWorldAnimate,
};