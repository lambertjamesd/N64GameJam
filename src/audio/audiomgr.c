
#include <ultralog.h>
#include "audio.h"
#include "src/boot.h"

typedef union {    

    struct {
        short     type;
    } gen;

    struct {
        short     type;
        struct    AudioInfo_s *info;
    } done;
    
    OSScMsg       app;
    
} AudioMsg;

typedef struct AudioInfo_s {
    short         *data;         
    short         frameSamples;  
    OSScTask      task;          
    AudioMsg      msg;           
} AudioInfo;

typedef struct {
    Acmd          *ACMDList[NUM_ACMD_LISTS];
    AudioInfo     *audioInfo[NUM_OUTPUT_BUFFERS];
    OSThread      thread;
    OSMesgQueue   audioFrameMsgQ;
    OSMesg        audioFrameMsgBuf[MAX_AUDIO_MESGS];
    OSMesgQueue   audioReplyMsgQ;
    OSMesg        audioReplyMsgBuf[MAX_AUDIO_MESGS];
    ALGlobals     g;
} AMAudioMgr;

typedef struct 
{
    ALLink        node;
    u32           startAddr;
    u32           lastFrame;
    char          *ptr;
} AMDMABuffer;

typedef struct 
{
    u8            initialized;
    AMDMABuffer   *firstUsed;
    AMDMABuffer   *firstFree;
} AMDMAState;


extern OSSched         gScheduler;
extern OSMesgQueue     *gSchedulerCommandQ;

AMAudioMgr      __am;
static u64      audioStack[AUDIO_STACKSIZE/sizeof(u64)];

AMDMAState      dmaState;
AMDMABuffer     dmaBuffs[NUM_DMA_BUFFERS];
u32             audFrameCt = 0;
u32             nextDMA = 0;
u32             lastDMA = 0;
u32             curAcmdList = 0;
u32             minFrameSize;
u32             frameSize;
u32             maxFrameSize;
u32             maxRSPCmds;

OSIoMesg        audDMAIOMesgBuf[NUM_DMA_MESSAGES];
OSMesgQueue     audDMAMessageQ;
OSMesg          audDMAMessageBuf[NUM_DMA_MESSAGES];

static void __amMain(void *arg);
static s32  __amDMA(s32 addr, s32 len, void *state);
static ALDMAproc __amDmaNew(AMDMAState **state);
static u32  __amHandleFrameMsg(AudioInfo *, AudioInfo *);
static void __amHandleDoneMsg(AudioInfo *);
static void __clearAudioDMA(void);

void amCreateAudioMgr(ALSynConfig *c, OSPri pri, amConfig *amc, int fps)
{
    u32     i;
    f32     fsize;

    dmaState.initialized = FALSE;

    c->dmaproc    = __amDmaNew;    
    c->outputRate = osAiSetFrequency(amc->outputRate);

    fsize = (f32) amc->framesPerField * c->outputRate / (f32) fps;
    frameSize = (s32) fsize;
    if (frameSize < fsize)
        frameSize++;
    if (frameSize & 0xf)
        frameSize = (frameSize & ~0xf) + 0x10;
    minFrameSize = frameSize - 16;
    maxFrameSize = frameSize + EXTRA_SAMPLES + 16;

    alInit(&__am.g, c);

    dmaBuffs[0].node.prev = 0; 
    dmaBuffs[0].node.next = 0;
    for (i=0; i<NUM_DMA_BUFFERS-1; i++)
    {
        alLink((ALLink*)&dmaBuffs[i+1],(ALLink*)&dmaBuffs[i]);
        dmaBuffs[i].ptr = alHeapAlloc(c->heap, 1, DMA_BUFFER_LENGTH);
    }

    dmaBuffs[i].ptr = alHeapAlloc(c->heap, 1, DMA_BUFFER_LENGTH);
    
    for(i=0;i<NUM_ACMD_LISTS;i++)
        __am.ACMDList[i] = (Acmd*)alHeapAlloc(c->heap, 1, 
                            amc->maxACMDSize * sizeof(Acmd));

    maxRSPCmds = amc->maxACMDSize;

    for (i = 0; i < NUM_OUTPUT_BUFFERS; i++) 
    {
        __am.audioInfo[i] = (AudioInfo *)alHeapAlloc(c->heap, 1,
                                                     sizeof(AudioInfo));
        __am.audioInfo[i]->msg.done.type = OS_SC_DONE_MSG;
        __am.audioInfo[i]->msg.done.info = __am.audioInfo[i];
        __am.audioInfo[i]->data = alHeapAlloc(c->heap, 1, 4*maxFrameSize);
    }    
    
    osCreateMesgQueue(&__am.audioReplyMsgQ, __am.audioReplyMsgBuf, MAX_AUDIO_MESGS);
    osCreateMesgQueue(&__am.audioFrameMsgQ, __am.audioFrameMsgBuf, MAX_AUDIO_MESGS);
    osCreateMesgQueue(&audDMAMessageQ, audDMAMessageBuf, NUM_DMA_MESSAGES);

    osCreateThread(&__am.thread, 3, __amMain, 0,
                   (void *)(audioStack+AUDIO_STACKSIZE/sizeof(u64)), pri);
    osStartThread(&__am.thread);
}

static void __amMain(void *arg) 
{
    u32         validTask;
    u32         done = 0;
    AudioMsg    *msg;
    AudioInfo   *lastInfo = 0;
    OSScClient  client;


    osScAddClient(&gScheduler, &client, &__am.audioFrameMsgQ);
    
    while (!done) 
    {
        (void) osRecvMesg(&__am.audioFrameMsgQ, (OSMesg *)&msg, OS_MESG_BLOCK);

        switch (msg->gen.type) 
        {
            case (OS_SC_RETRACE_MSG):
                validTask = __amHandleFrameMsg(__am.audioInfo[audFrameCt % 3],
                                               lastInfo);
                if(validTask)
                {
                    osRecvMesg(&__am.audioReplyMsgQ, (OSMesg *)&msg, 
                               OS_MESG_BLOCK);
                    __amHandleDoneMsg(msg->done.info);
                    lastInfo = msg->done.info;
                }
                break;

            case (OS_SC_PRE_NMI_MSG):
                break;

            case (QUIT_MSG):
                done = 1;
                break;

            default:
                break;
        }        
    }
    
    alClose(&__am.g);
}

static u32 __amHandleFrameMsg(AudioInfo *info, AudioInfo *lastInfo)
{
    s16 *audioPtr;
    Acmd *cmdp;
    s32 cmdLen;
    int samplesLeft = 0;
    OSScTask *t;

    
    __clearAudioDMA();

    audioPtr = (s16 *) osVirtualToPhysical(info->data);
    
    if (lastInfo)
        osAiSetNextBuffer(lastInfo->data, lastInfo->frameSamples<<2);

    
    samplesLeft = osAiGetLength() >> 2;

    info->frameSamples = 16 + (frameSize - samplesLeft + EXTRA_SAMPLES)& ~0xf;
    if(info->frameSamples < minFrameSize)
        info->frameSamples = minFrameSize;

    cmdp = alAudioFrame(__am.ACMDList[curAcmdList], &cmdLen, audioPtr,
                        info->frameSamples);

    
    if(cmdLen == 0)
        return 0;

    t = &info->task;
    
    t->next      = 0;                    
    t->msgQ      = &__am.audioReplyMsgQ; 
    t->msg       = (OSMesg)&info->msg;   
    t->flags     = OS_SC_NEEDS_RSP;
    
    t->list.t.data_ptr    = (u64 *) __am.ACMDList[curAcmdList];
    t->list.t.data_size   = (cmdp - __am.ACMDList[curAcmdList]) * sizeof(Acmd);
    t->list.t.type  = M_AUDTASK;
    t->list.t.ucode_boot = (u64 *)rspbootTextStart;
    t->list.t.ucode_boot_size =
        ((int) rspbootTextEnd - (int) rspbootTextStart);
    t->list.t.flags  = OS_TASK_DP_WAIT;
    t->list.t.ucode = (u64 *) aspMainTextStart;
    t->list.t.ucode_data = (u64 *) aspMainDataStart;
    t->list.t.ucode_data_size = SP_UCODE_DATA_SIZE;
    t->list.t.dram_stack = (u64 *) NULL;
    t->list.t.dram_stack_size = 0;
    t->list.t.output_buff = (u64 *) NULL;
    t->list.t.output_buff_size = 0;
    t->list.t.yield_data_ptr = NULL;
    t->list.t.yield_data_size = 0;

    osSendMesg(gSchedulerCommandQ, (OSMesg) t, OS_MESG_BLOCK);
    
    curAcmdList ^= 1;
    
    return 1;
}

static void __amHandleDoneMsg(AudioInfo *info) 
{
    s32    samplesLeft;
    static int firstTime = 1;

    samplesLeft = osAiGetLength()>>2;
    if (samplesLeft == 0 && !firstTime) 
    {
        firstTime = 0;
    }
}

s32 __amDMA(s32 addr, s32 len, void *state)
{
    void            *foundBuffer;
    s32             delta, addrEnd, buffEnd;
    AMDMABuffer     *dmaPtr, *lastDmaPtr;
    
    lastDmaPtr = 0;
    dmaPtr = dmaState.firstUsed;
    addrEnd = addr+len;
    
    while(dmaPtr)
    {
        buffEnd = dmaPtr->startAddr + DMA_BUFFER_LENGTH;
        if(dmaPtr->startAddr > addr) 
            break;                  

        else if(addrEnd <= buffEnd) 
        {
            dmaPtr->lastFrame = audFrameCt; 
            foundBuffer = dmaPtr->ptr + addr - dmaPtr->startAddr;
            return (int) osVirtualToPhysical(foundBuffer);
        }
        lastDmaPtr = dmaPtr;
        dmaPtr = (AMDMABuffer*)dmaPtr->node.next;
    }

    dmaPtr = dmaState.firstFree;

    if(!dmaPtr || nextDMA == NUM_DMA_BUFFERS) {
	    return osVirtualToPhysical(dmaState.firstUsed);
    }

    dmaState.firstFree = (AMDMABuffer*)dmaPtr->node.next;
    alUnlink((ALLink*)dmaPtr);

    if(lastDmaPtr)
    { 
        alLink((ALLink*)dmaPtr,(ALLink*)lastDmaPtr);
    }
    else if(dmaState.firstUsed)
    {
        lastDmaPtr = dmaState.firstUsed;
        dmaState.firstUsed = dmaPtr;
        dmaPtr->node.next = (ALLink*)lastDmaPtr;
        dmaPtr->node.prev = 0;
        lastDmaPtr->node.prev = (ALLink*)dmaPtr;
    }
    else
    {
        dmaState.firstUsed = dmaPtr;
        dmaPtr->node.next = 0;
        dmaPtr->node.prev = 0;
    }
    
    foundBuffer = dmaPtr->ptr;
    delta = addr & 0x1;
    addr -= delta;
    dmaPtr->startAddr = addr;
    dmaPtr->lastFrame = audFrameCt;

    audDMAIOMesgBuf[nextDMA].hdr.pri      = OS_MESG_PRI_NORMAL;
    audDMAIOMesgBuf[nextDMA].hdr.retQueue = &audDMAMessageQ;
    audDMAIOMesgBuf[nextDMA].dramAddr     = foundBuffer;
    audDMAIOMesgBuf[nextDMA].devAddr      = (u32)addr;
    audDMAIOMesgBuf[nextDMA].size         = DMA_BUFFER_LENGTH;

    osEPiStartDma(handler, &audDMAIOMesgBuf[nextDMA++], OS_READ);

    return (int) osVirtualToPhysical(foundBuffer) + delta;
}

ALDMAproc __amDmaNew(AMDMAState **state)
{
    int         i;
    
    if(!dmaState.initialized)
    {
        dmaState.firstUsed = 0;
        dmaState.firstFree = &dmaBuffs[0];
        dmaState.initialized = 1;
    }

    *state = &dmaState;

    return __amDMA;
}

static void __clearAudioDMA(void)
{
    u32          i;
    OSIoMesg     *iomsg;
    AMDMABuffer  *dmaPtr,*nextPtr;
    
    for (i=0; i<nextDMA; i++)
    {
        if (osRecvMesg(&audDMAMessageQ,(OSMesg *)&iomsg,OS_MESG_NOBLOCK) == -1);
    }

    
    dmaPtr = dmaState.firstUsed;
    while(dmaPtr)
    {
        nextPtr = (AMDMABuffer*)dmaPtr->node.next;

        if(dmaPtr->lastFrame + FRAME_LAG  < audFrameCt) 
        {
            if(dmaState.firstUsed == dmaPtr)
                dmaState.firstUsed = (AMDMABuffer*)dmaPtr->node.next;
            alUnlink((ALLink*)dmaPtr);
            if(dmaState.firstFree)
                alLink((ALLink*)dmaPtr,(ALLink*)dmaState.firstFree);
            else
            {
                dmaState.firstFree = dmaPtr;
                dmaPtr->node.next = 0;
                dmaPtr->node.prev = 0;
            }
        }
        dmaPtr = nextPtr;
    }
    
    lastDMA = nextDMA;
    nextDMA = 0;
    audFrameCt++;
}

