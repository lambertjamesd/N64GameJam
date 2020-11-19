
#ifndef _EFFECTS_TUTORIAL_H
#define _EFFECTS_TUTORIAL_H

#include "src/time/time.h"

enum TutorialMenuType {
    TutorialMenuMove,
    TutorialMenuJump,
    TutorialMenuRobot,
    TutorialMenuSwitch,
    TutorialMenuTypeCount,
};

struct TutorialMenu {
    float currTime;
    float delay;
    enum TutorialMenuType type;
    struct TimeUpdateListener updateListener;
    short state;
};

void tutorialMenuInit(struct TutorialMenu* tutorial, enum TutorialMenuType type);

extern struct TutorialMenu gTutorialMenu;

void tutorialMenuCheck();

#endif