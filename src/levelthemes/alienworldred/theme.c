
#include "theme.h"
#include "header.h"
#include "src/time/time.h"

Gfx* _gAlienWorldRedMaterials[] = {
    _alienredFloor_material,
    0,
    0,
    _alienredWall_material,
    _alienredUnderhang_material,
    _alienred_swamp_material_anim,
    _alienred_track_material_anim,
};

Gfx* _gAlienWorldRedDynamicMaterials[] = {
    _alienred_switch_material,
    _alienred_puzzle_door_material,
    _alienred_breakable_material,
    _alienred_moving_platform_material,
    _entrance_exit_red_material,
    _entrance_exit_red_off_material,
};

Gfx* _gAlienWorldRedDynamicMaterialCleanup[] = {
    0,
    0,
    _alienred_breakable_material_revert,
    0,
    _entrance_exit_red_material_revert,
    _entrance_exit_red_material_revert,
};

#define SCROLL_SPEED_X 0.3f
#define SCROLL_SPEED_Y 0.2f

static float gAlienScrollX = 0.0f;
static float gAlienScrollY = 0.0f;

void alienRedWorldAnimate(void* themeBufferStart) {
    u32 swampStart = ((u32)_alienred_swamp_material_anim) & 0xFFFFFF;
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

    gSPDisplayList(texDL++, _alienred_swamp_material);
    gDPSetTileSize(texDL++, 0, xOff, yOff, xOff + 124, yOff + 124);
    gSPEndDisplayList(texDL++);

    osWritebackDCache((void*)swampStart, sizeof(Gfx) * 3);
}

struct LevelThemeGraphics gAlienWorldRedLevelTheme = {
    _gAlienWorldRedMaterials,
    sizeof(_gAlienWorldRedMaterials) / sizeof(*_gAlienWorldRedMaterials),
    _gAlienWorldRedDynamicMaterials,
    _gAlienWorldRedDynamicMaterialCleanup,
    sizeof(_gAlienWorldRedDynamicMaterials) / sizeof(*_gAlienWorldRedDynamicMaterials),
    _alienred_clearScreen,
    _alienred_clearScreenPal,
    {
        _alienred_lswitch_up_tri, 
        _alienred_lswitch_down_tri, 
        _alienred_sswitch_up_tri, 
        _alienred_sswitch_down_tri, 
        _alienred_puzzle_door_mesh_tri_0,
        _alienred_rock_small_tri,
        _alienred_rock_big_tri,
        _alienred_rock_fragment_tri,
        _alienred_moving_platform_tri_0,
        _entrance_exit_red_mesh_tri_0,
    },
    alienRedWorldAnimate,
};