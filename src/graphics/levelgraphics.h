
#ifndef _GRAPHICS_LEVELGRAPHICS_H
#define _GRAPHICS_LEVELGRAPHICS_H

#include <ultra64.h>

struct LevelTileGrid {
    // Gfx* tiles[x][y][materialIndex]
    Gfx**** tiles;
    int sizeX;
    int sizeY;
    int materialCount;
    int gridSize;
};

struct LevelGraphics {
    struct LevelTileGrid grid;
};

#endif