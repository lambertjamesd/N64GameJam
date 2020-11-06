
#ifndef _TIME_TIME_H
#define _TIME_TIME_H

#include <ultra64.h>

typedef void (*UpdateCallback)(void* data);

struct TimeUpdateListener {
    struct TimeUpdateListener* prev;
    struct TimeUpdateListener* next;
    UpdateCallback callback;
    void* data;
};

extern float gTimeDelta;

#define MIN_DELTA_TIME (1.0f / 60.0f)

void timeUpdate(OSTime time);
void timeAddListener(struct TimeUpdateListener* listener, UpdateCallback callback, void* data);
void timeRemoveListener(struct TimeUpdateListener* listener);

#endif