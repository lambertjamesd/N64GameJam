
#include "memory.h"

static unsigned int gHeapStart;
static unsigned int gHeapEnd;
static unsigned int gHeapCurrent;

void heapInit(unsigned int start, unsigned int end)
{
    gHeapStart = start;
    gHeapEnd = end;
    gHeapCurrent = end;
}

void heapReset()
{
    gHeapCurrent = gHeapEnd;
}

void* heapMalloc(unsigned int len, unsigned int align)
{
    gHeapCurrent -= len;
    gHeapCurrent = gHeapCurrent & ~(align - 1);
    void* result = (void*)gHeapCurrent;

    return result;
}