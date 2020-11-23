
#include "controller.h"
#include "src/system/assert.h"

static OSMesg gDummyMessage;
OSMesgQueue gContMessageQ;
OSContStatus gControllerStatus[MAXCONTROLLERS];
OSContPad gControllerState[MAXCONTROLLERS];
u16 gControllerLastButton[MAXCONTROLLERS];

#define OUTER_DEAD_ZONE     10

int gInputRange[MAXCONTROLLERS] = {
    64,
    64,
    64,
    64,
};

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

int getButton(int controller, int mask) {
    return gControllerState[0].button & mask;
}

int getButtonDown(int controller, int mask) {
    return gControllerState[0].button & ~gControllerLastButton[controller] & mask;
}

int getButtonUp(int controller, int mask) {
    return ~gControllerState[0].button & gControllerLastButton[controller] & mask;
}

extern struct Vector2 getJoystick(int controller) {
    struct Vector2 result;
    
    s8 rawX = gControllerState[0].stick_x;

    if (rawX > gInputRange[controller]) {
        gInputRange[controller] = rawX;
    } else if (rawX < -gInputRange[controller]) {
        gInputRange[controller] = -rawX;
    }

    s8 rawY = gControllerState[0].stick_y;

    if (rawY > gInputRange[controller]) {
        gInputRange[controller] = rawY;
    } else if (rawY < -gInputRange[controller]) {
        gInputRange[controller] = -rawY;
    }

    float scale = 1.0f / (float)(gInputRange[controller] - OUTER_DEAD_ZONE);

    result.x = rawX * scale;
    result.y = -rawY * scale;

    if (result.x < -1.0f) {
        result.x = -1.0f;
    } else if (result.x > 1.0f) {
        result.x = 1.0f;
    }

    if (result.y < -1.0f) {
        result.y = -1.0f;
    } else if (result.y > 1.0f) {
        result.y = 1.0f;
    }

    return result;
}