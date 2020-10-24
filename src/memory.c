
#include "memory.h"

static int gHeapStart;
static int gHeapEnd;
static int gHeapCurrent;

void heapInit(int start, int end)
{
    gHeapStart = start;
    gHeapEnd = end;
    gHeapCurrent = start;
}

void heapReset()
{
    gHeapCurrent = gHeapStart;
}

void* heapMalloc(int len, int align)
{
    gHeapCurrent = (gHeapCurrent + align - 1) & ~(align - 1);
    void* result = (void*)gHeapCurrent;
    gHeapCurrent += len;

    return result;
}