
#ifndef _INPUT_CONTROLLER_H
#define _INPUT_CONTROLLER_H

#include <ultra64.h>
#include "src/math/vector2.h"

extern OSContStatus gControllerStatus[MAXCONTROLLERS];
extern OSContPad gControllerState[MAXCONTROLLERS];

extern void controllersInit();
extern void controllersReadData();

extern int getButton(int controller, int mask);
extern int getButtonDown(int controller, int mask);
extern int getButtonUp(int controller, int mask);

extern struct Vector2 getJoystick(int controller);

#endif