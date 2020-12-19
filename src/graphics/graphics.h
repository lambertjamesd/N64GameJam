
#ifndef _RENDER_RENDER_H
#define _RENDER_RENDER_H

#include <sched.h>
#include "levelgraphics.h"
#include "levelthemegraphics.h"
#include "renderscene.h"
#include "src/defs.h"
#include "src/font/fontrenderer.h"

#define DYANAMIC_LIST_LEN 612
#define DYNAMIC_MATRIX_COUNT 128
#define LOOK_AT_COUNT   8

typedef void (*MenuRenderCallback)(void* data, struct GraphicsState* state, struct FontRenderer* fontRenderer);

typedef struct {
	Mtx	projection[MAX_VIEWPORTS];
    u16 perspectiveCorrect[MAX_VIEWPORTS];
	Mtx	viewing[MAX_VIEWPORTS];
    Mtx worldScale[MAX_VIEWPORTS];
    Vp viewports[MAX_VIEWPORTS];
	Gfx	glist[DYANAMIC_LIST_LEN];
    Mtx dynamicActors[DYNAMIC_MATRIX_COUNT];
    LookAt lookats[LOOK_AT_COUNT];
    struct FontRenderer fontRenderer;
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

extern Gfx rdpstateinit_dl_pal[];
extern Gfx	rdpstateinit_dl[];
extern Gfx	setup_rspstate[];
extern Gfx 	logo_dl[];
extern Gfx 	bg_dl[];

extern unsigned short* gColorBuffer[2];
extern unsigned short	gZBuffer[];

extern u64	gRSPYieldBuffer[];

extern u64          gDramStack[];

extern int gScreenHeight;
extern float gScreenYScale;
extern short gUnusedDL;
extern short gUnusedMatrices;

#define SCALE_FOR_PAL(input) ((gScreenHeight == SCREEN_HT_NTSC) ? (input) : ((input) * SCREEN_HT_PAL/SCREEN_HT_NTSC))
#define UNSCALE_FOR_PAL(input) ((gScreenHeight == SCREEN_HT_NTSC) ? (input) : ((input) * SCREEN_HT_NTSC/SCREEN_HT_PAL))

void graphicsInitLevel(
    char* staticSegment, 
    char* levelSegment, 
    char* themeSegment, 
    struct LevelGraphics* levelGraphics, 
    struct LevelThemeGraphics* theme
);

void graphicsAddMenu(MenuRenderCallback renderCallback, void* data, int priority);
void graphicsRemoveMenu(MenuRenderCallback renderCallback, void* data);
void graphicsClearMenus();

#endif /* _RENDER_RENDER_H */
