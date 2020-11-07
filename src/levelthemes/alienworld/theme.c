
#include "theme.h"
#include "header.h"

Gfx* _gAlienWorldMaterials[] = {
    _alienFloor_material,
    0,
    0,
    _alienWall_material,
    _alienUnderhang_material,
};

Gfx* _gAlienWorldDynamicMaterials[] = {
    _alien_switch_material,
    _alien_puzzle_door_material,
    _alien_breakable_material,
};

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
};