
#include "theme.h"
#include "header.h"
#include "src/time/time.h"

Gfx* _gAlienWorldGreyMaterials[] = {
    _aliengreyFloor_material,
    0,
    0,
    _aliengreyWall_material,
    _aliengreyUnderhang_material,
    _aliengrey_swamp_material_anim,
    _aliengrey_track_material_anim,
};

Gfx* _gAlienWorldGreyDynamicMaterials[] = {
    _aliengrey_switch_material,
    _aliengrey_puzzle_door_material,
    _aliengrey_breakable_material,
    _aliengrey_moving_platform_material,
    _entrance_exit_grey_material,
    _entrance_exit_grey_off_material,
};

Gfx* _gAlienWorldGreyDynamicMaterialCleanup[] = {
    0,
    0,
    _aliengrey_breakable_material_revert,
    0,
    _entrance_exit_grey_material_revert,
    _entrance_exit_grey_material_revert,
};

#define SCROLL_SPEED_X 0.3f
#define SCROLL_SPEED_Y 0.2f

static float gAlienScrollX = 0.0f;
static float gAlienScrollY = 0.0f;

void alienGreyWorldAnimate(void* themeBufferStart) {
    u32 swampStart = ((u32)_aliengrey_swamp_material_anim) & 0xFFFFFF;
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

    gSPDisplayList(texDL++, _aliengrey_swamp_material);
    gDPSetTileSize(texDL++, 0, xOff, yOff, xOff + 124, yOff + 124);
    gSPEndDisplayList(texDL++);

    osWritebackDCache((void*)swampStart, sizeof(Gfx) * 3);
}

struct LevelThemeGraphics gAlienWorldGreyLevelTheme = {
    _gAlienWorldGreyMaterials,
    sizeof(_gAlienWorldGreyMaterials) / sizeof(*_gAlienWorldGreyMaterials),
    _gAlienWorldGreyDynamicMaterials,
    _gAlienWorldGreyDynamicMaterialCleanup,
    sizeof(_gAlienWorldGreyDynamicMaterials) / sizeof(*_gAlienWorldGreyDynamicMaterials),
    _aliengrey_clearScreen,
    _aliengrey_clearScreenPal,
    {
        _aliengrey_lswitch_up_tri, 
        _aliengrey_lswitch_down_tri, 
        _aliengrey_sswitch_up_tri, 
        _aliengrey_sswitch_down_tri, 
        _aliengrey_puzzle_door_mesh_tri_0,
        _aliengrey_rock_small_tri,
        _aliengrey_rock_big_tri,
        _aliengrey_rock_fragment_tri,
        _aliengrey_moving_platform_tri_0,
        _entrance_exit_grey_mesh_tri_0,
    },
    alienGreyWorldAnimate,
};