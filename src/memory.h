
#ifndef _MEMORY_H
#define _MOMORY_H

#include <ultra64.h>

#define ALIGN_64_BYTES(input)   (((input) + 0x3f) & ~0x3f)

void heapInit(unsigned int start, unsigned int end);
void heapReset();
void* heapMalloc(unsigned int len, unsigned int align);

void tlbAlign(unsigned int len, u32* alignedSize, OSPageMask* pageMask);

#endif