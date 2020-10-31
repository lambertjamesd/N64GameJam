
#include "theme.h"
#include "header.h"

Gfx* _gAlienWorldMaterials[] = {
    _alienFloor_material,
    0,
    0,
    _alienWall_material,
    _alienUnderhang_material,
};

struct LeveThemeGraphics gAlienWorldLevelTheme = {
    _gAlienWorldMaterials,
    sizeof(_gAlienWorldMaterials) / sizeof(*_gAlienWorldMaterials),
    46, 14, 89,
};