
#ifndef _INPUT_INPUTFOCUS_H
#define _INPUT_INPUTFOCUS_H

enum InputMask {
    InputMaskCamera = 1 << 0,
    InputMaskFreeCamera = 1 << 1,
    InputMaskPlayer = 1 << 2,
    InputMaskPauseMenu = 1 << 3,
};

#define INPUT_MASK_PLAY (InputMaskCamera | InputMaskPlayer)
#define INPUT_MASK_FREE_CAM (InputMaskCamera | InputMaskFreeCamera)

#define MAX_PLAYER_COUNT 2

extern int gInputMask[MAX_PLAYER_COUNT];

void inputMaskPush(int state);
void inputMaskPop();

#endif