
#include "menu.h"
#include "src/input/controller.h"
#include "src/font/endlessbossbattle/endlessbossbattle.h"
#include "src/audio/playersounds.h"
#include "src/audio/audio.h"

#define ANIMATE_DURATION     0.5f
#define MAX_LIST_DISPLAY      8

void menuListGetPage(struct MenuItemGroup* group, int selected) {

}

void menuGroupRender(struct GraphicsState* state, struct FontRenderer* fontRenderer, struct MenuItemGroup* group, int selected, int scrollOffset, int xOffset) {
    float halfWidth = fontRendererMeasureWidth(&gEndlessBossBattle, group->title);
    
    gDPPipeSync(state->dl++);
    gSPDisplayList(state->dl++, gEndlessBossBattleUse);
    gDPSetEnvColor(state->dl++, 255, 255, 255, 255);
    fontRendererSetScale(fontRenderer, 2.0f, 2.0f*gScreenYScale);
    fontRendererDrawCharacters(
        fontRenderer,
        &gEndlessBossBattle,
        &state->dl,
        group->title,
        (SCREEN_WD/2) - (int)halfWidth + xOffset, (gScreenHeight >> 1) - SCALE_FOR_PAL(80)
    );

    int y = (gScreenHeight >> 1) - 40;
    int yStep = SCALE_FOR_PAL(40);
    int x = SCREEN_WD/2;
    float scale = 2.0f;

    if (group->itemCount > 3 || group->type == MenuTypeList) {
        fontRendererSetScale(fontRenderer, 2.0f, 2.0f*gScreenYScale);
        yStep = SCALE_FOR_PAL(12);
        scale = 1.0f;
    } else if (group->itemCount == 1) {
        y += 40;
    }

    fontRendererSetScale(fontRenderer, scale, scale*gScreenYScale);

    int i;
    for (i = 0; i < group->itemCount; i++) {
        int sectionWidth = (int)(fontRendererMeasureWidth(
            &gEndlessBossBattle, 
            group->items[i].text
        ) * scale);
        int currentX = ((SCREEN_WD - sectionWidth)/2);
        if (currentX < x) {
            x = currentX;
        }
    }

    int yStart = y;
    x += xOffset;

    if (group->type == MenuTypeList) {
        char buffer[2];
        if (scrollOffset > 0) {
            buffer[0] = EBBUpArrow;
            buffer[1] = '\0';
            fontRendererDrawCharacters(
                fontRenderer,
                &gEndlessBossBattle,
                &state->dl,
                buffer,
                x, y - 11
            );
        }

        if (scrollOffset + MAX_LIST_DISPLAY < group->itemCount) {
            buffer[0] = EBBDownArrow;
            buffer[1] = '\0';
            fontRendererDrawCharacters(
                fontRenderer,
                &gEndlessBossBattle,
                &state->dl,
                buffer,
                x, y + MAX_LIST_DISPLAY * yStep
            );
        }
    }

    for (i = 0; i+scrollOffset < group->itemCount && i < MAX_LIST_DISPLAY; i++) {
        if (selected == i+scrollOffset) {
            gDPSetEnvColor(state->dl++, 255, 0, 255, 255);
        } else {
            gDPSetEnvColor(state->dl++, 255, 255, 255, 255);
        }

        fontRendererDrawCharacters(
            fontRenderer,
            &gEndlessBossBattle,
            &state->dl,
            group->items[i+scrollOffset].text,
            x, y
        );

        y += yStep;
    }

    if (group->renderMore) {
        group->renderMore(group, state, fontRenderer);
    }

    y = yStart;

    for (i = 0; i+scrollOffset < group->itemCount && i < MAX_LIST_DISPLAY; i++) {
        if (group->items[i+scrollOffset].renderMore) {
            group->items[i+scrollOffset].renderMore(
                group->items[i+scrollOffset].data, 
                x, 
                y,
                selected == i+scrollOffset,
                state,
                fontRenderer
            );
        }

        y += yStep;
    }
}

void menuRender(void* data, struct GraphicsState* state, struct FontRenderer* fontRenderer) {
    struct Menu* menu = (struct Menu*)data;

    if (menu->exitAnimationLevel != menu->insertAnimationLevel) {
        int offset = (int)(menu->animationTime * (SCREEN_WD / ANIMATE_DURATION));
        if (menu->exitAnimationLevel > menu->insertAnimationLevel) {
            if (menu->exitAnimationLevel != -1) {
                menuGroupRender(
                    state, 
                    fontRenderer, 
                    menu->current[menu->exitAnimationLevel], 
                    menu->selected[menu->exitAnimationLevel], 
                    menu->scrollOffset[menu->exitAnimationLevel],
                    offset
                );
            }

            if (menu->insertAnimationLevel != -1) {
                menuGroupRender(
                    state, 
                    fontRenderer, 
                    menu->current[menu->insertAnimationLevel], 
                    menu->selected[menu->insertAnimationLevel], 
                    menu->scrollOffset[menu->insertAnimationLevel],
                    offset - SCREEN_WD
                );
            }
        } else {
            if (menu->exitAnimationLevel != -1) {
                menuGroupRender(
                    state, 
                    fontRenderer, 
                    menu->current[menu->exitAnimationLevel], 
                    menu->selected[menu->exitAnimationLevel], 
                    menu->scrollOffset[menu->exitAnimationLevel],
                    -offset
                );
            }

            if (menu->insertAnimationLevel != -1) {
                menuGroupRender(
                    state, 
                    fontRenderer, 
                    menu->current[menu->insertAnimationLevel], 
                    menu->selected[menu->insertAnimationLevel], 
                    menu->scrollOffset[menu->insertAnimationLevel],
                    -offset + SCREEN_WD
                );
            }
        }
    } else if (menu->itemStackDepth != -1) {
        menuGroupRender(
            state, 
            fontRenderer, 
            menu->current[menu->itemStackDepth], 
            menu->selected[menu->itemStackDepth], 
            menu->scrollOffset[menu->itemStackDepth],
            0
        );
    }

    gDPPipeSync(state->dl++);
}

void menuUpdate(struct Menu* menu) {
    if (menu->exitAnimationLevel != menu->insertAnimationLevel) {
        menu->animationTime += gTimeDelta;
        
        if (menu->animationTime >= ANIMATE_DURATION) {
            menu->exitAnimationLevel = menu->insertAnimationLevel;
            menu->animationTime = 0.0f;
        }
    }
}

void menuInit(struct Menu* menu, struct MenuItemGroup* rootMenu) {
    menu->itemStackDepth = 0;
    menu->selected[0] = 0;
    menu->current[0] = rootMenu;
    menu->scrollOffset[0] = 0;
    menu->lastAxis = 0;
    menu->insertAnimationLevel = 0;
    menu->exitAnimationLevel = -1;
    menu->animationTime = 0.0f;
}

void menuPush(struct Menu* menu, struct MenuItemGroup* items) {
    int depth = menu->itemStackDepth;
    if (depth + 1 < MAX_MENU_DEPTH) {
        menu->exitAnimationLevel = depth;
        depth++;
        menu->insertAnimationLevel = depth;
        menu->itemStackDepth = depth;
        menu->current[depth] = items;
        menu->selected[depth] = 0;
        menu->scrollOffset[depth] = 0;
        menu->animationTime = 0.0f;
    }
}

void menuGoBack(struct Menu* menu) {
    int depth = menu->itemStackDepth;

    if (depth > 0) {
        menu->exitAnimationLevel = depth;
        depth--;
        menu->insertAnimationLevel = depth;
        menu->itemStackDepth = depth;
        menu->animationTime = 0.0f;
    }
}

void menuHandleInput(struct Menu* menu, int controllerIndex) {
    if (menu->itemStackDepth == -1) {
        return;
    }

    int menuDir = menuGetInputDir(gControllerState[controllerIndex].stick_y, &menu->lastAxis, &menu->lastTimer);

    int depth = menu->itemStackDepth;

    if (getButtonDown(controllerIndex, A_BUTTON)) {
        struct MenuItem* selectedItem = &menu->current[depth]->items[menu->selected[depth]];
        switch (selectedItem->type) {
            case MenuItemMenu:
                menuPush(menu, selectedItem->targetMenu);
                depth = menu->itemStackDepth;
                break;
            case MenuItemBack:
                menuGoBack(menu);
                depth = menu->itemStackDepth;
                break;
            case MenuItemAction:
                selectedItem->action(menu, selectedItem->data);
                break;
        }
        audioRestartPlaySound(
            gPlayerSoundIds[SoundUISelect],
            0.5f,
            MENU_SELECT_SOUND_VOLUME,
            0.0f,
            10
        );
    } else if (getButtonDown(controllerIndex, B_BUTTON)) {
        menuGoBack(menu);
        depth = menu->itemStackDepth;
        audioRestartPlaySound(
            gPlayerSoundIds[SoundUISelect],
            0.5f,
            MENU_SELECT_SOUND_VOLUME,
            0.0f,
            10
        );
    }

    struct MenuItemGroup* current = menu->current[depth];

    if (menuDir > 0) {
        if (menu->selected[depth] > 0) {
            audioRestartPlaySound(
                gPlayerSoundIds[SoundUIScroll],
                0.5f,
                1.0f,
                0.0f,
                10
            );

            menu->selected[depth]--;

            int relativePosition = menu->selected[depth] - menu->scrollOffset[depth];

            if (current->type == MenuTypeList && 
                menu->scrollOffset[depth] > 0 && 
                relativePosition <= 0) {
                relativePosition = 1;
                menu->scrollOffset[depth] = menu->selected[depth] - relativePosition;
            } else if (relativePosition < 0) {
                relativePosition = 0;
                menu->scrollOffset[depth] = menu->selected[depth] - relativePosition;
            }
        }
    } else if (menuDir < 0) {
        if (menu->selected[depth] + 1 < current->itemCount) {
            audioRestartPlaySound(
                gPlayerSoundIds[SoundUIScroll],
                0.5f,
                1.0f,
                0.0f,
                10
            );

            menu->selected[depth]++;

            int relativePosition = menu->selected[depth] - menu->scrollOffset[depth];

            if (current->type == MenuTypeList &&
                menu->scrollOffset[depth] + MAX_LIST_DISPLAY <= current->itemCount &&
                relativePosition > (MAX_LIST_DISPLAY - 2)) {
                relativePosition = MAX_LIST_DISPLAY - 2;
                menu->scrollOffset[depth] = menu->selected[depth] - relativePosition;
            }
        }
    }

}

int menuCheckRepeartTimer(char* timer) {
    if (timer) {
        if (*timer == 0) {
            *timer = 8;
            return 1;
        } else {
            (*timer)--;
        }
    }

    return 0;
}

int menuGetInputDir(char axis, char* lastAxis, char* lastTimer) {
    int result = 0;

    if (axis < -32) {
        if (*lastAxis >= -32) {
            result = -1;

            if (lastTimer) {
                *lastTimer = 15;
            }
        } else if (menuCheckRepeartTimer(lastTimer)) {
            result = -1;
        }
    } else if (axis > 32) {
        if (*lastAxis <= 32) {
            result = 1;

            if (lastTimer) {
                *lastTimer = 15;
            }
        } else if (menuCheckRepeartTimer(lastTimer)) {
            result = 1;
        }
    }

    *lastAxis = axis;

    return result;
}