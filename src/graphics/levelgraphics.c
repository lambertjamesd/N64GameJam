
#include "levelgraphics.h"
#include "src/system/memory.h"

void graphicsRenderLevelTileGrid(struct LevelTileGrid* levelGrid, Gfx** materials, int materialCount, struct GraphicsState* state) {
    int materialIdx;
    int x;
    int y;

    char* isOnScreen = fastMalloc(levelGrid->sizeX*levelGrid->sizeY, sizeof(char));

    struct Box collisionBox;

    int tileGridSize = 2 * levelGrid->gridSize;

    collisionBox.min.y = -2.0f;
    collisionBox.max.y = 8.0f;

    for (x = 0; x < levelGrid->sizeX; ++x) {
        collisionBox.min.x = x * tileGridSize-1.0f;
        collisionBox.max.x = (x+1) * tileGridSize-1.0f;

        for (y = 0; y < levelGrid->sizeY; ++y) {
            collisionBox.min.z = -(y+1) * tileGridSize+1.0f;
            collisionBox.max.z = -y * tileGridSize+1.0f;

            int sectionIndex = y + x * levelGrid->sizeY;

            isOnScreen[sectionIndex] = 
                !planeIsBoxBehind(&state->frustumPlanes[0], &collisionBox) &&
                !planeIsBoxBehind(&state->frustumPlanes[1], &collisionBox) &&
                !planeIsBoxBehind(&state->frustumPlanes[2], &collisionBox) &&
                !planeIsBoxBehind(&state->frustumPlanes[3], &collisionBox);
        }
    }

    for (materialIdx = 0; materialIdx < materialCount && materialIdx < levelGrid->materialCount; ++materialIdx) {
        if (materials[materialIdx]) {
            gSPDisplayList(state->dl++, materials[materialIdx]);
        }

        for (x = 0; x < levelGrid->sizeX; ++x) {
            for (y = 0; y < levelGrid->sizeY; ++y) {
                if (levelGrid->tiles[x][y][materialIdx] && isOnScreen[y + x * levelGrid->sizeY]) {
                    gSPDisplayList(state->dl++, levelGrid->tiles[x][y][materialIdx]);
                }
            }
        }
    }

    fastMallocReset();
}