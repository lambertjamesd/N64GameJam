
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
};

#define SCROLL_SPEED_X 0.3f
#define SCROLL_SPEED_Y 0.2f

static float gAlienScrollX = 0.0f;
static float gAlienScrollY = 0.0f;

void alienWorldAnimate() {
    u32 physicalAddr = osVirtualToPhysical(_alien_swamp_material_anim);

    if (physicalAddr != -1) {
        Gfx* texDL = _alien_swamp_material_anim;

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

        osWritebackDCache(_alien_swamp_material_anim, sizeof(Gfx) * 3);
    }
}

struct LevelThemeGraphics gAlienWorldLevelTheme = {
    _gAlienWorldMaterials,
    sizeof(_gAlienWorldMaterials) / sizeof(*_gAlienWorldMaterials),
    _gAlienWorldDynamicMaterials,
    sizeof(_gAlienWorldDynamicMaterials) / sizeof(*_gAlienWorldDynamicMaterials),
    _alien_clearScreen,
    {
        _alien_lswitch_up_tri, 
        _alien_lswitch_down_tri, 
        _alien_sswitch_up_tri, 
        _alien_sswitch_down_tri, 
        _alien_puzzle_door_mesh_tri_0,
        _alien_puzzle_door_mesh_tri_0,
        _alien_breakable_mesh_tri_0,
    },
    alienWorldAnimate,
};