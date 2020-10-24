
#ifndef _MEMORY_H
#define _MOMORY_H

void heapInit(unsigned int start, unsigned int end);
void heapReset();
void* heapMalloc(unsigned int len, unsigned int align);

#endif