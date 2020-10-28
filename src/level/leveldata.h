
#ifndef _LEVEL_LEVELDATA_H
#define _LEVEL_LEVELDATA_H

#include <ultra64.h>

struct DrawingBatch {
    Gfx* displayList;
    int renderedAtFrame;
};

struct TileEntry {
    struct DrawingBatch* drawingBatch;
    
};

struct TileMap {

};

#endif