
#ifndef _CUTSCENE_JPEG_DECODER_H
#define _CUTSCENE_JPEG_DECODER_H

#include <ultra64.h>

extern OSMesgQueue gCompletedImages;
extern OSThread gJpegThread;

enum JpegDecodeState {
    JpegDecodeStateNew,
    JpegDecodeStateDecoding,
    JpegDecodeStateError,
    JpegDecodeStateDone,
};

struct JpegDecodeRequest {
    enum JpegDecodeState state;
    char* jpegSource;
    char* targetResult;
    char* currentRead;
    short width;
    short height;
};

void jpegDecoderStartThread();
void jpegDecode(char* jpegData, char* targetImageData);

#endif