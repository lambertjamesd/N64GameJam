
#include "pausemenu.h"
#include "src/input/inputfocus.h"
#include "src/input/controller.h"
#include "src/graphics/graphics.h"
#include "src/font/fontrenderer.h"
#include "src/font/endlessbossbattle/endlessbossbattle.h"
#include "menu.h"
#include "src/level/level.h"
#include "src/boot.h"
#include "src/save/savefile.h"
#include "src/audio/playersounds.h"
#include "src/audio/audio.h"
#include "src/strings/strings.h"

#define PAUSE_MENU_WIDTH    120
#define PAUSE_MENU_HEIGHT   160

#define LINE_HEIGHT         20

#define SOUND_STEP_COUNT    4

float gVolumeSettingSteps[SOUND_STEP_COUNT] = {1.0f, 0.666f, 0.333f, 0.0f};
int gSoundVolumeSetting = 0;
int gMusicVolumeSetting = 0;

enum StringIndex gSoundVolumeNames[SOUND_STEP_COUNT] = {
    STR_SOUND_HIGH,
    STR_SOUND_MEDIUM,
    STR_SOUND_LOW,
    STR_SOUND_OFF,
};

enum StringIndex gMusicVolumeNames[SOUND_STEP_COUNT] = {
    STR_MUSIC_HIGH,
    STR_MUSIC_MEDIUM,
    STR_MUSIC_LOW,
    STR_MUSIC_OFF,
};

enum StringIndex gPauseMenuText[PauseMenuItemCount] = {
    STR_RESUME,
    STR_RESTART_LEVEL,
    STR_SOUND_HIGH,
    STR_MUSIC_HIGH,
    STR_SAVE,
    STR_SAVE_AND_QUIT,
};

struct PauseMenu gPauseMenu;

struct Color gSelectColor = {MENU_SELECTED_COLOR};
struct Color gGoodColor = {MENU_GOOD_COLOR};
struct Color gBadColor = {MENU_BAD_COLOR};

void pauseMenuRender(void* data, struct GraphicsState* state, struct FontRenderer* fontRenderer) {
    struct PauseMenu* pauseMenu =  (struct PauseMenu*)data;

    int modifiedHeight = SCALE_FOR_PAL(PAUSE_MENU_HEIGHT);
    int modfiedLineHeight = SCALE_FOR_PAL(LINE_HEIGHT);

    gDPPipeSync(state->dl++);
	gDPSetCombineLERP(state->dl++, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1);
    gDPFillRectangle(
        state->dl++, 
        (SCREEN_WD-PAUSE_MENU_WIDTH)>>1, 
        (gScreenHeight-modifiedHeight)>>1,
        (SCREEN_WD+PAUSE_MENU_WIDTH)>>1, 
        (gScreenHeight+modifiedHeight)>>1
    );
    gDPPipeSync(state->dl++);

    gSPDisplayList(state->dl++, OS_K0_TO_PHYSICAL(gEndlessBossBattleUse));

    float halfWidth = fontRendererMeasureWidth(&gEndlessBossBattle, getStr(STR_PAUSED));

    fontRendererSetScale(fontRenderer, 2.0f, 2.0f * gScreenYScale);

    fontRendererDrawCharacters(
        fontRenderer,
        &gEndlessBossBattle,
        &state->dl,
        getStr(STR_PAUSED),
        (SCREEN_WD >> 1) - (int)(halfWidth),
        ((gScreenHeight - modifiedHeight) >> 1) + SCALE_FOR_PAL(10)
    );

    fontRendererSetScale(fontRenderer, 1.0f, gScreenYScale);

    int i;

    for (i = 0; i < PauseMenuItemCount; ++i) {
        int isSaved = i == PauseMenuItemSave && !saveFileNeedsSave();
        char* text = getStr(isSaved ? STR_SAVED : gPauseMenuText[i]);
        halfWidth = fontRendererMeasureWidth(&gEndlessBossBattle, text) * 0.5f;
        int yOffset = 20 + i * modfiedLineHeight - (int)((modfiedLineHeight * PauseMenuItemCount) >> 1);

        struct Color color = {MENU_TEXT_COLOR};

        if (i == PauseMenuItemSoundVolume) {
            text = getStr(gSoundVolumeNames[gSoundVolumeSetting]);

            if (gSoundVolumeSetting == SOUND_STEP_COUNT - 1) {
                color = gBadColor;
            }
        } else if (i == PauseMenuItemMusicVolume) {
            text = getStr(gMusicVolumeNames[gMusicVolumeSetting]);

            if (gMusicVolumeSetting == SOUND_STEP_COUNT - 1) {
                color = gBadColor;
            }
        }

        if (pauseMenu->selectedItem == i) {
            if (isSaved) {
                color = gGoodColor;
            } else {
                color = gSelectColor;
            }
        }

        gDPSetEnvColor(state->dl++, color.r, color.g, color.b, color.a);

        fontRendererDrawCharacters(
            fontRenderer,
            &gEndlessBossBattle,
            &state->dl,
            text,
            (SCREEN_WD >> 1) - (int)(halfWidth),
            (gScreenHeight >> 1) + yOffset
        );
    }
    gDPPipeSync(state->dl++);
}

void pauseMenuExit(struct PauseMenu* pauseMenu) {
    graphicsRemoveMenu(pauseMenuRender, pauseMenu);
    timeRemoveListener(&pauseMenu->updateListener, TimeUpdateGroupMenu);
    timeSetGroupDisabled(TimeUpdateGroupWorld, 0);
    inputMaskPop();
}

void pauseMenuUpdate(void* data) {
    struct PauseMenu* pauseMenu =  (struct PauseMenu*)data;

    if (pauseMenu->selectedItem == -1) {
        pauseMenu->selectedItem = 0;
        return;
    }

    int inputDir = menuGetInputDir(gControllerState[0].stick_y, &pauseMenu->lastVerticalInput, 0);

    if (inputDir > 0 && pauseMenu->selectedItem > 0) {
        pauseMenu->selectedItem--;
        audioRestartPlaySound(
            gPlayerSoundIds[SoundUIScroll],
            0.5f,
            1.0f,
            0.0f,
            10
        );
    } else if (inputDir < 0 && pauseMenu->selectedItem + 1 < PauseMenuItemCount) {
        pauseMenu->selectedItem++;
        audioRestartPlaySound(
            gPlayerSoundIds[SoundUIScroll],
            0.5f,
            1.0f,
            0.0f,
            10
        );
    }

    if (getButtonDown(0, START_BUTTON|B_BUTTON)) {
        audioRestartPlaySound(
            gPlayerSoundIds[SoundUISelect],
            0.5f,
            MENU_SELECT_SOUND_VOLUME,
            0.0f,
            10
        );
        pauseMenuExit(pauseMenu);
    } else if (getButtonDown(0, A_BUTTON)) {
        switch (pauseMenu->selectedItem) {
            case PauseMenuItemResume:
                pauseMenuExit(pauseMenu);
                break;
            case PauseMenuItemRestart:
                restartLevel();
                break;
            case PauseMenuItemSoundVolume:
                gSoundVolumeSetting = (gSoundVolumeSetting + 1) % SOUND_STEP_COUNT;
                audioSetSoundVolume(gVolumeSettingSteps[gSoundVolumeSetting]);
                break;
            case PauseMenuItemMusicVolume:
                gMusicVolumeSetting = (gMusicVolumeSetting + 1) % SOUND_STEP_COUNT;
                audioSetSeqVolume(gVolumeSettingSteps[gMusicVolumeSetting]);
                break;
            case PauseMenuItemSave:
                if (saveFileNeedsSave()) {
                    audioPlaySound(
                        gPlayerSoundIds[GoalTouchSmall],
                        0.5f,
                        0.8f,
                        0.0f,
                        10
                    );
                }
                saveFileSave();
                break;
            case PauseMenuItemMainMenu:
                saveFileSave();
                levelSetNext(SceneIndexMainMenu, 1);
                break;
        }
        audioRestartPlaySound(
            gPlayerSoundIds[SoundUISelect],
            0.5f,
            MENU_SELECT_SOUND_VOLUME,
            0.0f,
            10
        );
    }
}

void pauseMenuShow(struct PauseMenu* pauseMenu) {
    pauseMenu->selectedItem = -1;

    graphicsAddMenu(pauseMenuRender, pauseMenu, 1);

    inputMaskPush(InputMaskPauseMenu);
    timeSetGroupDisabled(TimeUpdateGroupWorld, 1);

    pauseMenu->lastVerticalInput = 0;

    timeAddListener(&pauseMenu->updateListener, pauseMenuUpdate, pauseMenu, TimeUpdateGroupMenu);
}