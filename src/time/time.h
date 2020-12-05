
#ifndef _TIME_TIME_H
#define _TIME_TIME_H

#include <ultra64.h>

typedef void (*UpdateCallback)(void* data);

enum TimeUpdateGroup {
    TimeUpdateGroupWorld,
    TimeUpdateGroupMenu,
    TimeUpdateGroupCount,
};

struct TimeUpdateListener {
    struct TimeUpdateListener* prev;
    struct TimeUpdateListener* next;
    UpdateCallback callback;
    void* data;
};

extern float gTimeDelta;

#define MIN_DELTA_TIME (1.0f / 60.0f)

void timeUpdate(OSTime time);
void timeAddListener(struct TimeUpdateListener* listener, UpdateCallback callback, void* data, enum TimeUpdateGroup group);
void timeRemoveListener(struct TimeUpdateListener* listener, enum TimeUpdateGroup group);
int timeHasListener(struct TimeUpdateListener* listener, enum TimeUpdateGroup group);
void timeResetListeners();

void timeSetGroupDisabled(enum TimeUpdateGroup group, int value);

#endif