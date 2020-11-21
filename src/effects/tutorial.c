
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

#define BAR_HEIGHT  30
#define BAR_Y       160
#define BAR_SIDE_PADDING 15

#define TEXT_SCALE      2.0f

#define ACCEL   ((BAR_HEIGHT + SCREEN_HT - BAR_Y) / (0.5f * ANIMATION_DURATION * ANIMATION_DURATION))

struct TutorialMenu gTutorialMenu;

enum ButtonFontMapping gTutorialButton[TutorialMenuTypeCount] = {
    ButtonFontMappingJoystick,
    ButtonFontMappingA,
    ButtonFontMappingB,
    ButtonFontMappingZ,
};

char* gTutorialText[] = {
    "Move",
    "Jump",
    "Attack",
    "Switch",
};

void tutorialRender(void* data, struct GraphicsState* state, struct FontRenderer* fontRenderer) {
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

    if (topY < SCREEN_HT) {
        if (bottomY > SCREEN_HT-1) {
            bottomY = SCREEN_HT-1;
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
        fontRendererSetScale(fontRenderer, TEXT_SCALE, TEXT_SCALE);
        float nextX = fontRendererDrawCharacters(
            fontRenderer, 
            &gButtonFont, 
            &state->dl, 
            buttonText, 
            SCREEN_WD/2-halfWidth, 
            (int)offset+BAR_Y-12
        );

        nextX += BAR_SIDE_PADDING;

        gSPDisplayList(state->dl++, gEndlessBossBattleUse);
        fontRendererDrawCharacters(
            fontRenderer, 
            &gEndlessBossBattle, 
            &state->dl, 
            gTutorialText[menu->type],
            (int)nextX,
            (int)offset+BAR_Y-8
        );

        gDPPipeSync(state->dl++);
    }

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


int tutorialDidPlayerMove() {
    return (gInputMask & InputMaskCadet) &&
            (gControllerState[0].stick_x != 0 ||
            gControllerState[0].stick_y != 0);
}

int tutorialDidPlayerJump() {
    return (gInputMask & InputMaskCadet) && getButtonDown(0, A_BUTTON);
}

int tutorialDidPlayerAttack() {
    return (gInputMask & InputMaskRobot) && getButtonDown(0, B_BUTTON);
}

int tutorialDidPlayerSwitch() {
    return (gLevelFlags & LEVEL_HAS_ROBOT) &&
        (gInputMask & (InputMaskRobot | InputMaskCadet)) && 
        getButtonDown(0, Z_TRIG | R_TRIG);
}

void tutorialMenuCheck() {
    if (tutorialDidPlayerMove()) {
        if (!(gSaveFile.tutorialFlags & SAVEFILE_LEARNED_MOVE)) {
            gTutorialMenu.delay = 0.0f;
        }

        gSaveFile.tutorialFlags |= SAVEFILE_LEARNED_MOVE;

        if (gTutorialMenu.state == 1 && gTutorialMenu.type == TutorialMenuMove) {
            gTutorialMenu.state = 2;
        }
    }

    if (tutorialDidPlayerJump()) {
        if (!(gSaveFile.tutorialFlags & SAVEFILE_LEARNED_JUMP)) {
            gTutorialMenu.delay = 0.0f;
        }

        gSaveFile.tutorialFlags |= SAVEFILE_LEARNED_JUMP;

        if (gTutorialMenu.state == 1 && gTutorialMenu.type == TutorialMenuJump) {
            gTutorialMenu.state = 2;
        }
    }

    if (tutorialDidPlayerAttack()) {
        if (!(gSaveFile.tutorialFlags & SAVEFILE_LEARNED_ATTACK)) {
            gTutorialMenu.delay = 0.0f;
        }

        gSaveFile.tutorialFlags |= SAVEFILE_LEARNED_ATTACK;

        if (gTutorialMenu.state == 1 && gTutorialMenu.type == TutorialMenuRobot) {
            gTutorialMenu.state = 2;
        }
    }

    if (tutorialDidPlayerSwitch()) {
        if (!(gSaveFile.tutorialFlags & SAVEFILE_LEARNED_SWITCH)) {
            gTutorialMenu.delay == 0.0f;
        }

        gSaveFile.tutorialFlags |= SAVEFILE_LEARNED_SWITCH;

        if (gTutorialMenu.state == 1 && gTutorialMenu.type == TutorialMenuSwitch) {
            gTutorialMenu.state = 2;
        }
    }

    if (!gTutorialMenu.state) {
        if (!(gSaveFile.tutorialFlags & SAVEFILE_LEARNED_MOVE)) {
            if (gTutorialMenu.delay < TUTORIAL_DELAY) {
                gTutorialMenu.delay += gTimeDelta;
            } else {
                tutorialMenuInit(&gTutorialMenu, TutorialMenuMove);
            }
        } else if (!(gSaveFile.tutorialFlags & SAVEFILE_LEARNED_JUMP)) {
            if (gTutorialMenu.delay < TUTORIAL_DELAY) {
                gTutorialMenu.delay += gTimeDelta;
            } else {
                tutorialMenuInit(&gTutorialMenu, TutorialMenuJump);
            }
        } else if (!(gSaveFile.tutorialFlags & SAVEFILE_LEARNED_FOUND_ROBOT)) {
            if ((gLevelFlags & LEVEL_INTRO_ROBOT) && 
                fabsf(gRobot.transform.position.x - gCadet.transform.position.x) < DISCOVER_RADIUS &&
                fabsf(gRobot.transform.position.z - gCadet.transform.position.z) < DISCOVER_RADIUS) {
                    levelSwitchToRobot();
                    levelFocusCutscene(&gRobot.transform.position, 2.0f);
                    gSaveFile.tutorialFlags |= SAVEFILE_LEARNED_FOUND_ROBOT;
                    gTutorialMenu.delay = 0.0f;
                    gLevelFlags |= LEVEL_HAS_ROBOT;
            }
        } else if ((gInputMask & InputMaskRobot) && !(gSaveFile.tutorialFlags & SAVEFILE_LEARNED_ATTACK)) {
            if (gTutorialMenu.delay < TUTORIAL_DELAY) {
                gTutorialMenu.delay += gTimeDelta;
            } else {
                tutorialMenuInit(&gTutorialMenu, TutorialMenuRobot);
            }
        } else if ((gInputMask & (InputMaskRobot | InputMaskCadet)) && !(gSaveFile.tutorialFlags & SAVEFILE_LEARNED_SWITCH)) {
            if (gTutorialMenu.delay < TUTORIAL_DELAY) {
                gTutorialMenu.delay += gTimeDelta;
            } else {
                tutorialMenuInit(&gTutorialMenu, TutorialMenuSwitch);
            }
        }
    }
}