
#include "memory.h"

static unsigned int gHeapStart;
static unsigned int gHeapEnd;
static unsigned int gHeapCurrent;

void heapInit(unsigned int start, unsigned int end)
{
    gHeapStart = start;
    gHeapEnd = end;
    gHeapCurrent = start;
}

void heapReset()
{
    gHeapCurrent = gHeapStart;
}

void* heapMalloc(unsigned int len, unsigned int align)
{
    gHeapCurrent = (gHeapCurrent + align - 1) & ~(align - 1);
    void* result = (void*)gHeapCurrent;
    gHeapCurrent += len;

    return result;
}