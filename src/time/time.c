
#include "time.h"

#define MAX_DELTA_TIME      0.1f

float gTimeDelta = 1.0f / 30.0f;
OSTime gTimeLast;
struct TimeUpdateListener* gTimeListenerHead[TimeUpdateGroupCount];
char gTimeGroupDisabled[TimeUpdateGroupCount];

void timeUpdate(OSTime currTime) {
    OSTime timeDelta = currTime - gTimeLast;
    gTimeDelta = (float)OS_CYCLES_TO_USEC(timeDelta) / 1000000.0f;

    if (gTimeDelta > MAX_DELTA_TIME) {
        gTimeDelta = MAX_DELTA_TIME;
    }

    gTimeLast = currTime;

    int i;

    for (i = 0; i < TimeUpdateGroupCount; ++i) {
        if (!gTimeGroupDisabled[i]) {
            struct TimeUpdateListener* currListener = gTimeListenerHead[i];

            while (currListener) {
                // get next listener here in case this one gets removed in the callback
                struct TimeUpdateListener* nextListener = currListener->next;

                currListener->callback(currListener->data);

                currListener = nextListener;
            }
        }
    }
}

void timeAddListener(struct TimeUpdateListener* listener, UpdateCallback callback, void* data, enum TimeUpdateGroup group) {
    if (gTimeListenerHead[group]) {
        gTimeListenerHead[group]->prev = listener;
    }
    listener->next = gTimeListenerHead[group];
    listener->prev = 0;
    listener->data = data;
    listener->callback = callback;
    gTimeListenerHead[group] = listener;
}

void timeRemoveListener(struct TimeUpdateListener* listener, enum TimeUpdateGroup group) {
    if (listener == gTimeListenerHead[group]) {
        gTimeListenerHead[group] = listener->next;

        if (gTimeListenerHead[group]) {
            gTimeListenerHead[group]->prev = 0;
        }
        
        listener->next = 0;
    } else {
        listener->prev->next = listener->next;

        if (listener->next) {
            listener->next->prev = listener->prev;
        }

        listener->next = 0;
        listener->prev = 0;
    }
}

void timeResetListeners() {
    int i;

    for (i = 0; i < TimeUpdateGroupCount; ++i) {
        gTimeListenerHead[i] = 0;
        gTimeGroupDisabled[i] = 0;
    }
}

void timeSetGroupDisabled(enum TimeUpdateGroup group, int value) {
    gTimeGroupDisabled[group] = value;
}