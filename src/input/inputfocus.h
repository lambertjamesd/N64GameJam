
#ifndef _INPUT_INPUTFOCUS_H
#define _INPUT_INPUTFOCUS_H

enum InputMask {
    InputMaskCamera = 1 << 0,
    InputMaskCadet = 1 << 1,
    InputMaskRobot = 1 << 2,
};

#define INPUT_MASK_CADET (InputMaskCamera | InputMaskCadet)
#define INPUT_MASK_ROBOT (InputMaskCamera | InputMaskRobot)

extern int gInputMask;

#endif