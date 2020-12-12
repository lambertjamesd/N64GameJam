
#include "tutorial.h"
#include "src/graphics/graphics.h"
#include "src/font/endlessbossbattle/endlessbossbattle.h"
#include "src/font/buttons/buttons.h"
#include "src/save/savefile.h"
#include "src/input/inputfocus.h"
#include "src/input/controller.h"
#include "src/level/level.h"
#include "src/robot/robot.h"
#include "src/cadet/cadet.h"

#define ANIMATION_DURATION      0.5f

#define TUTORIAL_DELAY          3.0f

#define DISCOVER_RADIUS         8.0f

// BAR_HEIGHT + 240 - 160 = 0.5 * ACCEL * ANIMATION_DURATION * ANIMATION_DURATION

#define BAR_HEIGHT  SCALE_FOR_PAL(30)
#define BAR_Y       ((gScreenHeight >> 1) + SCALE_FOR_PAL(40))
#define BAR_SIDE_PADDING 15

#define TEXT_SCALE      2.0f

#define ACCEL   ((BAR_HEIGHT + gScreenHeight - BAR_Y) / (0.5f * ANIMATION_DURATION * ANIMATION_DURATION))

struct TutorialMenu gTutorialMenu;

enum ButtonFontMapping gTutorialButton[TutorialMenuTypeCount] = {
    ButtonFontMappingJoystick,
    ButtonFontMappingA,
    ButtonFontMappingB,
    ButtonFontMappingZ,
    ButtonFontMappingC_U,
    ButtonFontMappingR,
    ButtonFontMappingStart,
};

char* gTutorialText[TutorialMenuTypeCount] = {
    "Move",
    "Jump",
    "Attack",
    "Switch",
    "Camera",
    "Map",
    "Skip",
};

void tutorialRender(void* data, struct GraphicsState* state, struct FontRenderer* fontRenderer) {
    if (gInputMask[0] & InputMaskPlayer) {
        struct TutorialMenu* menu = (struct TutorialMenu*)data;
        char buttonText[2];
        buttonText[0] = gTutorialButton[menu->type];
        buttonText[1] = 0;

        float menuWidth = (fontRendererMeasureWidth(&gButtonFont, buttonText) + 
            fontRendererMeasureWidth(&gEndlessBossBattle, gTutorialText[menu->type])) * TEXT_SCALE + BAR_SIDE_PADDING;

        int halfWidth = (int)menuWidth / 2;

        float offset = 0.5f * menu->currTime * menu->currTime * ACCEL;

        int topY = (int)(offset + BAR_Y-(BAR_HEIGHT>>1));
        int bottomY = (int)(offset + BAR_Y+(BAR_HEIGHT>>1));

        if (topY < gScreenHeight) {
            if (bottomY > gScreenHeight-1) {
                bottomY = gScreenHeight-1;
            }

            gDPPipeSync(state->dl++);
            gDPSetCycleType(state->dl++, G_CYC_FILL);
            gDPSetFillColor(state->dl++, (GPACK_RGBA5551(0, 0, 0, 1) << 16 | 
                        GPACK_RGBA5551(0, 0, 0, 1)));
            gDPFillRectangle(
                state->dl++, 
                SCREEN_WD/2-BAR_SIDE_PADDING - halfWidth, 
                topY,
                SCREEN_WD/2+BAR_SIDE_PADDING + halfWidth, 
                bottomY
            );
            gDPPipeSync(state->dl++);

            gSPDisplayList(state->dl++, gButtonFontUse);
            fontRendererSetScale(fontRenderer, TEXT_SCALE, TEXT_SCALE * gScreenYScale);
            float nextX = fontRendererDrawCharacters(
                fontRenderer, 
                &gButtonFont, 
                &state->dl, 
                buttonText, 
                SCREEN_WD/2-halfWidth, 
                (int)offset+BAR_Y-12
            );
            gDPPipeSync(state->dl++);

            nextX += BAR_SIDE_PADDING;

            gSPDisplayList(state->dl++, gEndlessBossBattleUse);
            fontRendererDrawCharacters(
                fontRenderer, 
                &gEndlessBossBattle, 
                &state->dl, 
                gTutorialText[menu->type],
                (int)nextX,
                (int)offset+BAR_Y-SCALE_FOR_PAL(8)
            );

            gDPPipeSync(state->dl++);
        }
    }
}

int tutorialMenuIsActive(struct TutorialMenu* menu) {
    return timeHasListener(&menu->updateListener, TimeUpdateGroupWorld);
}

void tutorialMenuUpdate(void* data) {
    struct TutorialMenu* menu = (struct TutorialMenu*)data;

    if (menu->state == 1 && menu->currTime > 0.0f) {
        menu->currTime -= gTimeDelta;
        
        if (menu->currTime < 0.0f) {
            menu->currTime = 0.0f;
        }
    } else if (menu->state == 2 && menu->currTime < ANIMATION_DURATION) {
        menu->currTime += gTimeDelta;

        if (menu->currTime >= ANIMATION_DURATION) {
            graphicsRemoveMenu(tutorialRender, menu);
            timeRemoveListener(&menu->updateListener, TimeUpdateGroupWorld);
            menu->state = 0;
        }
    }
}

void tutorialMenuInit(struct TutorialMenu* tutorial, enum TutorialMenuType type) {
    tutorial->currTime = ANIMATION_DURATION;
    tutorial->type = type;
    tutorial->state = 1;
    
    graphicsAddMenu(tutorialRender, tutorial, 0);
    timeAddListener(&tutorial->updateListener, tutorialMenuUpdate, tutorial, TimeUpdateGroupWorld);
}

void tutorialMenuClose(struct TutorialMenu* tutorial) {
    tutorial->state = 2;
}

int tutorialDidPlayerMove() {
    return gCadet.controllerIndex != -1 && (gInputMask[gCadet.controllerIndex] & InputMaskPlayer) &&
            (gControllerState[0].stick_x != 0 ||
            gControllerState[0].stick_y != 0);
}

int tutorialDidPlayerCamera() {
    return gCadet.controllerIndex != -1 && (gInputMask[gCadet.controllerIndex] & InputMaskPlayer) &&
            getButtonDown(gCadet.controllerIndex, R_CBUTTONS | L_CBUTTONS | U_CBUTTONS | D_CBUTTONS);
}

int tutorialDidPlayerCamFree() {
    return gCadet.controllerIndex != -1 && ((gInputMask[gCadet.controllerIndex] & InputMaskPlayer) &&
            getButtonDown(gCadet.controllerIndex, R_TRIG) || (gInputMask[gCadet.controllerIndex] & InputMaskFreeCamera));
}

int tutorialDidPlayerJump() {
    return getButtonDown(gCadet.controllerIndex, A_BUTTON) && (gInputMask[gCadet.controllerIndex] & InputMaskPlayer);
}

int tutorialDidPlayerAttack() {
    return getButtonDown(gRobot.controllerIndex, B_BUTTON) && (gInputMask[gRobot.controllerIndex] & InputMaskPlayer);
}

int tutorialDidPlayerSwitch() {
    return (gLevelFlags & LEVEL_HAS_ROBOT) &&
        (gInputMask[0] & InputMaskPlayer) && 
        getButtonDown(0, Z_TRIG | R_TRIG);
}

void tutorialMenuCheck() {
    if (tutorialDidPlayerMove()) {
        if (!saveFileCheckTutorial(SAVEFILE_LEARNED_MOVE)) {
            gTutorialMenu.delay = 0.0f;
        }

        saveFileMarkTutorial(SAVEFILE_LEARNED_MOVE);

        if (gTutorialMenu.state == 1 && gTutorialMenu.type == TutorialMenuMove) {
            tutorialMenuClose(&gTutorialMenu);
        }
    }

    if (tutorialDidPlayerJump()) {
        if (!saveFileCheckTutorial(SAVEFILE_LEARNED_JUMP)) {
            gTutorialMenu.delay = 0.0f;
        }

        saveFileMarkTutorial(SAVEFILE_LEARNED_JUMP);

        if (gTutorialMenu.state == 1 && gTutorialMenu.type == TutorialMenuJump) {
            tutorialMenuClose(&gTutorialMenu);
        }
    }

    if (tutorialDidPlayerCamera()) {
        if (!saveFileCheckTutorial(SAVEFILE_LEARNED_CAM_MOVE)) {
            gTutorialMenu.delay = 0.0f;
        }

        saveFileMarkTutorial(SAVEFILE_LEARNED_CAM_MOVE);

        if (gTutorialMenu.state == 1 && gTutorialMenu.type == TutorialMenuCamMove) {
            tutorialMenuClose(&gTutorialMenu);
        }
    }

    if (tutorialDidPlayerCamFree()) {
        if (!saveFileCheckTutorial(SAVEFILE_LEARNED_CAM_FREE)) {
            gTutorialMenu.delay = 0.0f;
        }

        saveFileMarkTutorial(SAVEFILE_LEARNED_CAM_FREE);

        if (gTutorialMenu.state == 1 && gTutorialMenu.type == TutorialMenuCamFree) {
            tutorialMenuClose(&gTutorialMenu);
        }
    }

    if (tutorialDidPlayerAttack()) {
        if (!saveFileCheckTutorial(SAVEFILE_LEARNED_ATTACK)) {
            gTutorialMenu.delay = -10.0f;
        }

        saveFileMarkTutorial(SAVEFILE_LEARNED_ATTACK);

        if (gTutorialMenu.state == 1 && gTutorialMenu.type == TutorialMenuRobot) {
            tutorialMenuClose(&gTutorialMenu);
        }
    }

    if (tutorialDidPlayerSwitch()) {
        if (!saveFileCheckTutorial(SAVEFILE_LEARNED_SWITCH)) {
            gTutorialMenu.delay == 0.0f;
        }

        saveFileMarkTutorial(SAVEFILE_LEARNED_SWITCH);

        if (gTutorialMenu.state == 1 && gTutorialMenu.type == TutorialMenuSwitch) {
            tutorialMenuClose(&gTutorialMenu);
        }
    }

    if (!gTutorialMenu.state) {
        if (!saveFileCheckTutorial(SAVEFILE_LEARNED_MOVE)) {
            if (gTutorialMenu.delay < TUTORIAL_DELAY) {
                gTutorialMenu.delay += gTimeDelta;
            } else {
                tutorialMenuInit(&gTutorialMenu, TutorialMenuMove);
            }
        } else if (!saveFileCheckTutorial(SAVEFILE_LEARNED_JUMP)) {
            if (gTutorialMenu.delay < TUTORIAL_DELAY) {
                gTutorialMenu.delay += gTimeDelta;
            } else {
                tutorialMenuInit(&gTutorialMenu, TutorialMenuJump);
            }
        } else if (!saveFileCheckTutorial(SAVEFILE_LEARNED_CAM_MOVE)) {
            if (gTutorialMenu.delay < TUTORIAL_DELAY) {
                gTutorialMenu.delay += gTimeDelta;
            } else {
                tutorialMenuInit(&gTutorialMenu, TutorialMenuCamMove);
            }
        } else if (!saveFileCheckTutorial(SAVEFILE_LEARNED_CAM_FREE)) {
            if (gTutorialMenu.delay < TUTORIAL_DELAY) {
                gTutorialMenu.delay += gTimeDelta;
            } else {
                tutorialMenuInit(&gTutorialMenu, TutorialMenuCamFree);
            }
        } else if (gRobot.controllerIndex != -1 && (gInputMask[gRobot.controllerIndex] & InputMaskPlayer) && !saveFileCheckTutorial(SAVEFILE_LEARNED_ATTACK)) {
            if (gTutorialMenu.delay < TUTORIAL_DELAY) {
                gTutorialMenu.delay += gTimeDelta;
            } else {
                tutorialMenuInit(&gTutorialMenu, TutorialMenuRobot);
            }
        } else if (gRobot.controllerIndex == 0 && (gInputMask[0] & InputMaskPlayer) && !saveFileCheckTutorial(SAVEFILE_LEARNED_SWITCH)) {
            if (gTutorialMenu.delay < TUTORIAL_DELAY) {
                gTutorialMenu.delay += gTimeDelta;
            } else {
                tutorialMenuInit(&gTutorialMenu, TutorialMenuSwitch);
            }
        }
    }

    if (!saveFileCheckTutorial(SAVEFILE_LEARNED_FOUND_ROBOT)) {
        if ((gLevelFlags & LEVEL_INTRO_ROBOT) && 
            fabsf(gRobot.transform.position.x - gCadet.transform.position.x) < DISCOVER_RADIUS &&
            fabsf(gRobot.transform.position.z - gCadet.transform.position.z) < DISCOVER_RADIUS) {
                int robotController = levelSwitchToRobot();
                levelFocusCutscene(&gRobot.transform.position, 2.0f, robotController);
                saveFileMarkTutorial(SAVEFILE_LEARNED_FOUND_ROBOT);
                gTutorialMenu.delay = 0.0f;
                gLevelFlags |= LEVEL_HAS_ROBOT;
        }
    }
}