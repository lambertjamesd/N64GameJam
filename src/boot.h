
#ifndef _BOOT_H
#define _BOOT_H

#include <ultra64.h>
#include "defs.h"
#include "src/graphics/graphics.h"

extern OSThread gGameThread;
extern OSThread gInitThread;

enum SceneIndex {
    SceneIndexSpinningLogo = -1,
    SceneIndexMainMenu = -2,
    SceneIndexIntroCutscene = -3,
};

#define MAX_MESGS	8
#define DMA_QUEUE_SIZE  200
#define NUM_FIELDS      1       /* 1 = 60 Hz video interrupts, 2 = 30 Hz, etc */

#define UTIL_PRIORITY		1
#define INIT_PRIORITY		10
#define GAME_PRIORITY		10
#define AUDIO_PRIORITY		12
#define SCHEDULER_PRIORITY	13

#define LOG_SCHEDULE_GFX_TASK	    101
#define LOG_RDP_DONE		    102
#define LOG_RETRACE		    103
#define LOG_INTR		    104

#define PRINTF(a) 

extern OSMesgQueue      gGfxFrameMsgQ;
extern OSMesg           gGfxFrameMsgBuf[MAX_MESGS];
extern OSPiHandle	*handler;

extern char _codeSegmentRomStart[], _codeSegmentRomEnd[];
extern char _staticSegmentRomStart[], _staticSegmentRomEnd[];
extern char _tableSegmentRomStart[], _tableSegmentRomEnd[];
extern char _seqSegmentRomStart[], _seqSegmentRomEnd[];
extern char _bankSegmentRomStart[], _bankSegmentRomEnd[];


void     audioInit();
void     graphicsInit(void);
void     createGfxTask(GFXInfo *i);
void     romCopy(const char *src, const char *dest, const int len);

#endif