
#include <ultra64.h>
#include <sched.h>
#include "game.h"
#include "audio/audio.h"
#include "src/graphics/graphics.h"
#include "boot.h"
#include "memory.h"
#include "src/debugger/debugger.h"
#include "src/input/controller.h"
#include "src/graphics/renderscene.h"
#include "src/time/time.h"
#include "defs.h"

OSThread gGameThread;
OSThread gInitThread;

/**** Stack for boot code.  Space can be reused after 1st thread starts ****/
u64    bootStack[STACKSIZEBYTES/sizeof(u64)];

/**** Stacks for the threads, divide by 8 which is the size of a u64 ****/
static u64      gGameThreadStack[STACKSIZEBYTES/sizeof(u64)];
static u64      gInitThreadStack[STACKSIZEBYTES/sizeof(u64)];

/**** function prototypes for private functions in this file ****/
static void     gameEntryPoint(void *);
static void     idleThreadEntryPoint(char *);
static void     initGame(void);

/**** message queues and message buffers used by this app ****/
static OSMesg           PiMessages[DMA_QUEUE_SIZE];
static OSMesgQueue      PiMessageQ;
static OSMesgQueue      gDMAMessageQ;
static OSMesg           gDMAMessageBuf;

/**** Graphics variables used in this file ****/
OSMesgQueue     gGfxFrameMsgQ;
OSMesg          gGfxFrameMsgBuf[MAX_MESGS];
Gfx             *glistp;
GFXInfo         gInfo[2];

/**** Scheduler globals ****/
OSSched         gScheduler;
OSMesgQueue	*gSchedulerCommandQ;
u64             scheduleStack[OS_SC_STACKSIZE/8];
OSScClient      gfxClient;


OSPiHandle	*handler;

void boot(void *arg)
{
    osInitialize();

    handler = osCartRomInit();
    
    osCreateThread(&gInitThread, 1, (void(*)(void *))idleThreadEntryPoint, arg,
                  (void *)(gInitThreadStack+(STACKSIZEBYTES/sizeof(u64))), 
		   (OSPri)INIT_PRIORITY);

    osStartThread(&gInitThread);
}

static void idleThreadEntryPoint(char *argv) 
{
    osCreatePiManager((OSPri) OS_PRIORITY_PIMGR, &PiMessageQ, PiMessages,
                        DMA_QUEUE_SIZE);
    osCreateThread(&gGameThread, 6, gameEntryPoint, argv, gGameThreadStack + 
		   (STACKSIZEBYTES/sizeof(u64)), (OSPri)GAME_PRIORITY);

    osStartThread(&gGameThread);
    osSetThreadPri(0, 0);
    for(;;);
}

static void gameEntryPoint(void *argv)
{
    u32         drawbuffer = 0;
    u32         pendingGFX = 0;
    GFXMsg      *msg = NULL;

    initGame();

    while (1) 
    {
        (void) osRecvMesg(&gGfxFrameMsgQ, (OSMesg *)&msg, OS_MESG_BLOCK);

        switch (msg->gen.type) 
        {
            case (OS_SC_RETRACE_MSG):            
                if (pendingGFX < 2) 
                {
                    createGfxTask(&gInfo[drawbuffer]);
                    pendingGFX++;
                    drawbuffer ^= 1;
                }

                controllersReadData();
                timeUpdate(osGetTime());
                break;

            case (OS_SC_DONE_MSG):
                pendingGFX--;
                break;
                
            case (OS_SC_PRE_NMI_MSG):
                pendingGFX += 2;
                break;
        }
    }
}

void romCopy(const char *src, const char *dest, const int len)
{
    OSIoMesg dmaIoMesgBuf;
    OSMesg dummyMesg;
    
    osInvalDCache((void *)dest, (s32) len);

    dmaIoMesgBuf.hdr.pri      = OS_MESG_PRI_NORMAL;
    dmaIoMesgBuf.hdr.retQueue = &gDMAMessageQ;
    dmaIoMesgBuf.dramAddr     = (void*)dest;
    dmaIoMesgBuf.devAddr      = (u32)src;
    dmaIoMesgBuf.size         = (u32)len;

    osEPiStartDma(handler, &dmaIoMesgBuf, OS_READ);
    (void) osRecvMesg(&gDMAMessageQ, &dummyMesg, OS_MESG_BLOCK);
}

static void layoutMemory(void)
{
    gColorBuffer[0] = (u16*)PHYS_TO_K0(osMemSize - 2 * sizeof(u16) * SCREEN_WD * SCREEN_HT);
    gColorBuffer[1] = (u16*)PHYS_TO_K0(osMemSize - sizeof(u16) * SCREEN_WD * SCREEN_HT);
    gAudioHeap = (u8*)PHYS_TO_K0(osMemSize - 2 * sizeof(u16) * SCREEN_WD * SCREEN_HT - AUDIO_HEAP_SIZE);
    heapInit((u32)PHYS_TO_K0(_codeSegmentRomEnd), (u32)gAudioHeap);
}

static void initGame(void)
{ 

    osCreateMesgQueue(&gDMAMessageQ, &gDMAMessageBuf, 1);
    osCreateMesgQueue(&gGfxFrameMsgQ, gGfxFrameMsgBuf, MAX_MESGS);

    osCreateScheduler(&gScheduler, (void *)(scheduleStack + OS_SC_STACKSIZE/8),
                      SCHEDULER_PRIORITY, OS_VI_NTSC_LAN1, NUM_FIELDS);

    osScAddClient(&gScheduler, &gfxClient, &gGfxFrameMsgQ);  

#if DEBUG
    OSThread* threadPtr[1];
    threadPtr[0] = &gGameThread;
    enum GDBError err = gdbInitDebugger(handler, &gDMAMessageQ, threadPtr, 1);
#else
    enum GDBError err = gdbSerialInit(handler, &gDMAMessageQ);
#endif

    gSchedulerCommandQ = osScGetCmdQ(&gScheduler);

    cameraInit(&gMainCamera);
    controllersInit();
    layoutMemory();
    graphicsInit(); 
    audioInit();
}

