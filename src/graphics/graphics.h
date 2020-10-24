
#ifndef _RENDER_RENDER_H
#define _RENDER_RENDER_H

#include <sched.h>

#define	SCREEN_HT	240
#define	SCREEN_WD	320

typedef struct {
	Mtx	projection;
    u16 perspectiveCorrect;
	Mtx	viewing;
    Mtx bg_model;
	Gfx	glist[512];
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

extern Dynamic	dynamic;

extern Gfx	rdpstateinit_dl[];
extern Gfx	setup_rdpstate[];
extern Gfx	setup_rspstate[];
extern Gfx 	logo_dl[];
extern Gfx 	bg_dl[];

extern Gfx	*glistp;

extern unsigned short* gColorBuffer[2];
extern unsigned short	gZBuffer[];

extern u64	gRSPYieldBuffer[];

extern u64          gDramStack[];

#endif /* _RENDER_RENDER_H */
