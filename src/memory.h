
#ifndef _MEMORY_H
#define _MOMORY_H

void heapInit(int start, int end);
void heapReset();
void* heapMalloc(int len, int align);

#endif