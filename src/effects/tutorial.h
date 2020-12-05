
#ifndef _EFFECTS_TUTORIAL_H
#define _EFFECTS_TUTORIAL_H

#include "src/time/time.h"

enum TutorialMenuType {
    TutorialMenuMove,
    TutorialMenuJump,
    TutorialMenuRobot,
    TutorialMenuSwitch,
    TutorialMenuCamMove,
    TutorialMenuCamFree,
    TutorialMenuSkip,
    TutorialMenuTypeCount,
};

struct TutorialMenu {
    float currTime;
    float delay;
    enum TutorialMenuType type;
    struct TimeUpdateListener updateListener;
    short state;
};

int tutorialMenuIsActive(struct TutorialMenu* menu);
void tutorialMenuInit(struct TutorialMenu* tutorial, enum TutorialMenuType type);
void tutorialMenuClose(struct TutorialMenu* tutorial);

extern struct TutorialMenu gTutorialMenu;

void tutorialMenuCheck();

#endif