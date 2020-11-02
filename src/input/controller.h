
#ifndef _INPUT_CONTROLLER_H
#define _INPUT_CONTROLLER_H

#include <ultra64.h>

extern OSContStatus gControllerStatus[MAXCONTROLLERS];
extern OSContPad gControllerState[MAXCONTROLLERS];

extern void controllersInit();
extern void controllersReadData();

extern int getButton(int controller, int mask);
extern int getButtonDown(int controller, int mask);
extern int getButtonUp(int controller, int mask);

#endif