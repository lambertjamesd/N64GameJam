
#ifndef _MEMORY_H
#define _MOMORY_H

#include <ultra64.h>
#include <stddef.h>

#define ALIGN_64_BYTES(input)   (((input) + 0x3f) & ~0x3f)

#define ALIGNMENT_OF(structType) (offsetof(struct {char a; structType b;}, b))

#define ARRAY_SIZE(structType, elementcount) (((sizeof(structType) + (ALIGNMENT_OF(structType) - 1)) & ~(ALIGNMENT_OF(structType) - 1)) * elementcount)

void heapInit(unsigned int start, unsigned int end);
void heapReset();
void* heapMalloc(unsigned int len, unsigned int align);

void tlbAlign(unsigned int len, u32* alignedSize, OSPageMask* pageMask);

void* fastMalloc(int len, int align);
void fastMallocReset();

#define FAST_MALLOC_STRUCT(structType) (fastMalloc(sizeof(structType), ALIGNMENT_OF(structType)))
#define FAST_MALLOC_STRUCT_ARRAY(structType, count) (fastMalloc(((sizeof(structType) + (ALIGNMENT_OF(structType) - 1)) & ~(ALIGNMENT_OF(structType) - 1)) * count, ALIGNMENT_OF(structType)))

#endif