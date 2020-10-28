
#include "controller.h"
#include "src/system/assert.h"

static OSMesg gDummyMessage;
OSMesgQueue gContMessageQ;
OSContStatus gControllerStatus[MAXCONTROLLERS];
OSContPad gControllerState[MAXCONTROLLERS];
u16 gControllerLastButton[MAXCONTROLLERS];

void controllersInit() {
    u8 pattern;
    osCreateMesgQueue(&gContMessageQ, &gDummyMessage, 1);
    osSetEventMesg(OS_EVENT_SI, &gContMessageQ, (OSMesg) 0);

    osContInit(&gContMessageQ, &pattern, &gControllerStatus[0]);

    assert((pattern & 1) && !(gControllerStatus[0].errno & CONT_NO_RESPONSE_ERROR));

    osContStartReadData(&gContMessageQ);
}

void controllersReadData() {
    int i;
    
    for (i = 0; i < MAXCONTROLLERS; ++i) {
        gControllerLastButton[i] = gControllerState[i].button;
    }

    if (osRecvMesg(&gContMessageQ, &gDummyMessage, OS_MESG_NOBLOCK) == 0) {
        osContGetReadData(gControllerState);
        osContStartReadData(&gContMessageQ);
    }
}