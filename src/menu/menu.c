
#include "menu.h"
#include "src/input/controller.h"
#include "src/font/endlessbossbattle/endlessbossbattle.h"

#define ANIMATE_DURATION     0.5f

void menuGroupRender(struct GraphicsState* state, struct FontRenderer* fontRenderer, struct MenuItemGroup* group, int selected, int xOffset) {
    float halfWidth = fontRendererMeasureWidth(&gEndlessBossBattle, group->title);
    
    fontRendererSetScale(fontRenderer, 2.0f, 2.0f);
    fontRendererDrawCharacters(
        fontRenderer,
        &gEndlessBossBattle,
        &state->dl,
        group->title,
        (SCREEN_WD/2) - (int)halfWidth + xOffset, 40
    );

    int y = 80;
    int yStep = 40;
    int x = SCREEN_WD/2;
    float scale = 2.0f;

    if (group->itemCount > 3) {
        fontRendererSetScale(fontRenderer, 2.0f, 2.0f);
        yStep = 12;
        scale = 1.0f;
    }

    fontRendererSetScale(fontRenderer, scale, scale);

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

    for (i = 0; i < group->itemCount; i++) {
        if (selected == i) {
            gDPSetEnvColor(state->dl++, 255, 0, 255, 255);
        } else {
            gDPSetEnvColor(state->dl++, 255, 255, 255, 255);
        }

        fontRendererDrawCharacters(
            fontRenderer,
            &gEndlessBossBattle,
            &state->dl,
            group->items[i].text,
            x + xOffset, y
        );

        y += yStep;
    }
}

void menuRender(void* data, struct GraphicsState* state, struct FontRenderer* fontRenderer) {
    struct Menu* menu = (struct Menu*)data;

    gDPPipeSync(state->dl++);
    gSPDisplayList(state->dl++, gEndlessBossBattleUse);

    if (menu->exitAnimationLevel != menu->insertAnimationLevel) {
        int offset = (int)(menu->animationTime * (SCREEN_WD / ANIMATE_DURATION));
        if (menu->exitAnimationLevel > menu->insertAnimationLevel) {
            if (menu->exitAnimationLevel != -1) {
                menuGroupRender(
                    state, 
                    fontRenderer, 
                    menu->current[menu->exitAnimationLevel], 
                    menu->selected[menu->exitAnimationLevel], 
                    offset
                );
            }

            if (menu->insertAnimationLevel != -1) {
                menuGroupRender(
                    state, 
                    fontRenderer, 
                    menu->current[menu->insertAnimationLevel], 
                    menu->selected[menu->insertAnimationLevel], 
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
                    -offset
                );
            }

            if (menu->insertAnimationLevel != -1) {
                menuGroupRender(
                    state, 
                    fontRenderer, 
                    menu->current[menu->insertAnimationLevel], 
                    menu->selected[menu->insertAnimationLevel], 
                    -offset + SCREEN_WD
                );
            }
        }
    } else if (menu->itemStackDepth != -1) {
        menuGroupRender(state, fontRenderer, menu->current[menu->itemStackDepth], menu->selected[menu->itemStackDepth], 0);
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

    int menuDir = menuGetInputDir(gControllerState[controllerIndex].stick_y, &menu->lastAxis);

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
    } else if (getButtonDown(controllerIndex, B_BUTTON)) {
        menuGoBack(menu);
        depth = menu->itemStackDepth;
    }

    if (menuDir > 0) {
        if (menu->selected[depth] > 0) {
            menu->selected[depth]--;
        }
    } else if (menuDir < 0) {
        if (menu->selected[depth] + 1 < menu->current[depth]->itemCount) {
            menu->selected[depth]++;
        }
    }
}

int menuGetInputDir(char axis, char* lastAxis) {
    int result = 0;

    if (axis < -32 && *lastAxis >= -32) {
        result = -1;
    } else if (axis > 32 && *lastAxis <= 32) {
        result = 1;
    }

    *lastAxis = axis;

    return result;
}