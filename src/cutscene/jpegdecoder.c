
#include "jpegdecoder.h"
#include "src/boot.h"
#include "src/tjpeg/tjpgd.h"
#include "src/system/assert.h"
#include <memory.h>

#define JPEG_MEMORY_REQUIREMENT 3100
#define MAX_REQUEST_COUNT 2

OSMesgQueue gCompletedImages;
OSMesgQueue gPendingImages;
OSMesg gCompletedImagesMsg;
OSMesg gPendingImagesMsg[MAX_REQUEST_COUNT];
OSThread gJpegThread;
static u64 gJpegThreadStack[STACKSIZEBYTES/sizeof(u64)];
char gJpegDecodeMemory[JPEG_MEMORY_REQUIREMENT];
struct JpegDecodeRequest gPendingRequests[MAX_REQUEST_COUNT];
int gNextRequestBuffer;
int gIsDecoding;

unsigned int readJpeg(JDEC* jd,uint8_t* buffer,unsigned int amount) {
    struct JpegDecodeRequest* request = (struct JpegDecodeRequest*)jd->device;
    if (buffer) {
        memcpy(buffer, request->currentRead, amount);
    }
    request->currentRead += amount;
    return amount;
}

int writeJpeg(JDEC* jd, void* memory, JRECT* rect) {
    struct JpegDecodeRequest* request = (struct JpegDecodeRequest*)jd->device;
    short* input = (short*)memory;
    short* bufferRowStart = (short*)request->targetResult + rect->top * jd->width + rect->left;
    int x, y;
    for (y = rect->top; y <= rect->bottom; ++y) {
        short* row = bufferRowStart;
        for (x = rect->left; x <= rect->right; ++x) {
            *row = input[(x - rect->left) + ((y - rect->top) << 3)];
            ++row;
        }
        bufferRowStart += jd->width;
    }
    return 1;
}

void jpegThreadLoop(void* argv) {
    while (1) {
        struct JpegDecodeRequest* request;
        osRecvMesg(&gPendingImages, (OSMesg*)&request, OS_MESG_BLOCK);
        gIsDecoding = 1;
        request->state = JpegDecodeStateDecoding;
        JDEC jd;
        if (jd_prepare(&jd, readJpeg, gJpegDecodeMemory, JPEG_MEMORY_REQUIREMENT, request) == JDR_OK) {
            if (jd_decomp(&jd, writeJpeg, 0) == JDR_OK) {
                request->width = jd.width;
                request->height = jd.height;
                request->state = JpegDecodeStateDone;
                osWritebackDCache(request->targetResult, 2 * jd.width * jd.height);
            } else {
                request->state = JpegDecodeStateError;
            }
        } else {
            request->state = JpegDecodeStateError;
        }
        gIsDecoding = 0;
        osSendMesg(&gCompletedImages, request, OS_MESG_BLOCK);
    }
}  

void jpegDecoderStartThread() {
    osCreateMesgQueue(&gPendingImages, gPendingImagesMsg, MAX_REQUEST_COUNT);
    osCreateMesgQueue(&gCompletedImages, &gCompletedImagesMsg, 1);

    osCreateThread(
        &gJpegThread, 
        7, 
        jpegThreadLoop, 
        0, 
        gJpegThreadStack+(STACKSIZEBYTES/sizeof(u64)),
        GAME_PRIORITY - 1
    );
    osStartThread(&gJpegThread);
}

void jpegDecode(char* jpegData, char* targetImageData) {
    assert(jpegData != 0);
    assert(targetImageData != 0);
    struct JpegDecodeRequest* request = &gPendingRequests[gNextRequestBuffer];
    gNextRequestBuffer = (gNextRequestBuffer + 1) % MAX_REQUEST_COUNT;

    request->state = JpegDecodeStateNew;
    request->jpegSource = jpegData;
    request->targetResult = targetImageData;
    request->currentRead = jpegData;
    request->width = 0;
    request->height = 0;

    osSendMesg(&gPendingImages, request, OS_MESG_NOBLOCK);
}

void jpegDecoderFlush() {
    OSMesg msg;
    while (osRecvMesg(&gPendingImages, &msg, OS_MESG_NOBLOCK) != -1);
    while (osRecvMesg(&gCompletedImages, &msg, OS_MESG_NOBLOCK) != -1);

    if (gIsDecoding) {
        osRecvMesg(&gCompletedImages, &msg, OS_MESG_BLOCK);
    }
}