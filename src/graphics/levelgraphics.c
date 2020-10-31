
#include "levelgraphics.h"

Gfx* graphicsRenderLevelTileGrid(struct LevelTileGrid* levelGrid, Gfx** materials, int materialCount, Gfx* dl) {
    int materialIdx;
    int x;
    int y;

    for (materialIdx = 0; materialIdx < materialCount && materialIdx < levelGrid->materialCount; ++materialIdx) {
        if (materials[materialIdx]) {
            gSPDisplayList(dl++, materials[materialIdx]);
        }

        for (x = 0; x < levelGrid->sizeX; ++x) {
            for (y = 0; y < levelGrid->sizeY; ++y) {
                if (levelGrid->tiles[x][y][materialIdx]) {
                    gSPDisplayList(dl++, levelGrid->tiles[x][y][materialIdx]);
                }
            }
        }
    }

    return dl;
}