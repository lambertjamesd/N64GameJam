
#include "inputfocus.h"

#define MAX_INPUT_STACK 4

int gInputMask = INPUT_MASK_PLAY;
int gCurrStackPos;

int gInputMaskStack[MAX_INPUT_STACK];

void inputMaskPush(int state) {
    if (gCurrStackPos < MAX_INPUT_STACK) {
        gInputMaskStack[gCurrStackPos] = gInputMask;
        gInputMask = state;
        ++gCurrStackPos;
    }
}

void inputMaskPop() {
    if (gCurrStackPos > 0) {
        --gCurrStackPos;
        gInputMask = gInputMaskStack[gCurrStackPos];
    }
}