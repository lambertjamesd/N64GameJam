
#ifndef _RENDER_RENDER_H
#define _RENDER_RENDER_H

#include <sched.h>
#include "levelgraphics.h"
#include "levelthemegraphics.h"
#include "renderscene.h"
#include "src/defs.h"

#define DYANAMIC_LIST_LEN 512
#define DYNAMIC_MATRIX_COUNT 128

typedef struct {
	Mtx	projection;
    u16 perspectiveCorrect;
	Mtx	viewing;
    Mtx worldScale;
	Gfx	glist[DYANAMIC_LIST_LEN];
    Mtx dynamicActors[DYNAMIC_MATRIX_COUNT];
} Dynamic;

typedef union {    

    struct {
        short   type;
    } gen;
    
    struct {
        short   type;
    } done;
    
    OSScMsg      app;
    
} GFXMsg;

typedef struct {
    OSScTask    task;
    Dynamic     dp;
    GFXMsg      msg;
    u16		*cfb;
} GFXInfo;

extern Gfx	rdpstateinit_dl[];
extern Gfx	setup_rspstate[];
extern Gfx 	logo_dl[];
extern Gfx 	bg_dl[];

extern Gfx	*glistp;

extern unsigned short* gColorBuffer[2];
extern unsigned short	gZBuffer[];

extern u64	gRSPYieldBuffer[];

extern u64          gDramStack[];

extern int gFrameNumber;

void graphicsInitLevel(
    char* staticSegment, 
    char* levelSegment, 
    char* themeSegment, 
    struct LevelGraphics* levelGraphics, 
    struct LevelThemeGraphics* theme
);

#endif /* _RENDER_RENDER_H */
