
#ifndef _INPUT_CONTROLLER_H
#define _INPUT_CONTROLLER_H

#include <ultra64.h>

extern OSContStatus gControllerStatus[MAXCONTROLLERS];
extern OSContPad gControllerState[MAXCONTROLLERS];

extern void controllersInit();
extern void controllersReadData();

#endif