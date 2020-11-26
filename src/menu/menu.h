
#ifndef MENU_MENU_H
#define MENU_MENU_H

#define MAX_MENU_DEPTH 8

#include "src/graphics/graphics.h"

struct Menu;

typedef void (*MenuItemActionCallback)(struct Menu* menu, void* data);

enum MenuItemType {
    MenuItemMenu,
    MenuItemBack,
    MenuItemAction,
};

struct MenuItem {
    char* text;
    enum MenuItemType type;
    union {
        struct MenuItemGroup* targetMenu;
        MenuItemActionCallback action;
    };
    void* data;
};

struct MenuItemGroup {
    char *title;
    struct MenuItem* items;
    int itemCount;
};

struct Menu {
    struct MenuItemGroup* current[MAX_MENU_DEPTH];
    char selected[MAX_MENU_DEPTH];
    char itemStackDepth;
    char lastAxis;
    char insertAnimationLevel;
    char exitAnimationLevel;
    float animationTime;
};

void menuPush(struct Menu* menu, struct MenuItemGroup* items);
void menuGoBack(struct Menu* menu);
void menuInit(struct Menu* menu, struct MenuItemGroup* rootMenu);
void menuHandleInput(struct Menu* menu, int controllerIndex);

void menuRender(void* data, struct GraphicsState* state, struct FontRenderer* fontRenderer);
void menuUpdate(struct Menu* menu);

int menuGetInputDir(char axis, char* lastAxis);

#endif