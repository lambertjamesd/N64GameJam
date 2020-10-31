
#include "time.h"

float gTimeDelta = 1.0f / 30.0f;
OSTime gTimeLast;
struct TimeUpdateListener* gTimeListenerHead;

void timeUpdate(OSTime currTime) {
    OSTime timeDelta = currTime - gTimeLast;
    gTimeDelta = (float)OS_CYCLES_TO_USEC(timeDelta) / 1000000.0f;

    gTimeLast = currTime;

    struct TimeUpdateListener* currListener = gTimeListenerHead;

    while (currListener) {
        // get next listener here in case this one gets removed in the callback
        struct TimeUpdateListener* nextListener = currListener->next;

        currListener->callback(currListener->data);

        currListener = nextListener;
    }
}

void timeAddListener(struct TimeUpdateListener* listener, UpdateCallback callback, void* data) {
    if (gTimeListenerHead) {
        gTimeListenerHead->prev = listener;
    }
    listener->next = gTimeListenerHead;
    listener->prev = 0;
    listener->data = data;
    listener->callback = callback;
    gTimeListenerHead = listener;
}

void timeRemoveListener(struct TimeUpdateListener* listener) {
    if (listener == gTimeListenerHead) {
        gTimeListenerHead = listener->next;

        if (gTimeListenerHead) {
            gTimeListenerHead->prev = 0;
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