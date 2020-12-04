
#include "inputfocus.h"

#define MAX_INPUT_STACK 4

int gInputMask[MAX_PLAYER_COUNT] = {INPUT_MASK_PLAY, INPUT_MASK_PLAY};
int gCurrStackPos;

int gInputMaskStack[MAX_INPUT_STACK];

void inputMaskPush(int state) {
    if (gCurrStackPos+MAX_PLAYER_COUNT <= MAX_INPUT_STACK) {
        gInputMaskStack[gCurrStackPos] = gInputMask[0];
        gInputMaskStack[gCurrStackPos+1] = gInputMask[1];
        gInputMask[0] = state;
        gInputMask[1] = state;
        gCurrStackPos += MAX_PLAYER_COUNT;
    }
}

void inputMaskPop() {
    if (gCurrStackPos >= MAX_PLAYER_COUNT) {
        gCurrStackPos -= MAX_PLAYER_COUNT;
        gInputMask[0] = gInputMaskStack[gCurrStackPos];
        gInputMask[1] = gInputMaskStack[gCurrStackPos+1];
    }
}