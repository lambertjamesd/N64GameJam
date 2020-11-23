
#ifndef _INPUT_INPUTFOCUS_H
#define _INPUT_INPUTFOCUS_H

enum InputMask {
    InputMaskCamera = 1 << 0,
    InputMaskPlayer = 1 << 1,
    InputMaskPauseMenu = 1 << 3,
};

#define INPUT_MASK_PLAY (InputMaskCamera | InputMaskPlayer)

extern int gInputMask;

void inputMaskPush(int state);
void inputMaskPop();

#endif