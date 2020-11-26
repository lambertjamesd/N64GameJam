
#ifndef MENU_MENU_H
#define MENU_MENU_H

#define MAX_MENU_DEPTH 8

#include "src/graphics/graphics.h"

struct Menu;

typedef void (*MenuItemActionCallback)(struct Menu* menu, void* data);

typedef void (*MenuItemAdditionalRender)(void* data, int x, int y, int selected, struct GraphicsState* state, struct FontRenderer* fontRenderer);

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
        int popDistance;
    };
    void* data;
    MenuItemAdditionalRender renderMore;
    short inputShortcutMask;
};

enum MenuType {
    MenuTypeLarge,
    MenuTypeList,
};

struct MenuItemGroup;

typedef void (*MenuItemGroupAdditionalRender)(struct MenuItemGroup* group, struct GraphicsState* state, struct FontRenderer* fontRenderer);

struct MenuItemGroup {
    char *title;
    struct MenuItem* items;
    int itemCount;
    enum MenuType type;
    MenuItemGroupAdditionalRender renderMore;
};

struct Menu {
    struct MenuItemGroup* current[MAX_MENU_DEPTH];
    char selected[MAX_MENU_DEPTH];
    char scrollOffset[MAX_MENU_DEPTH];
    char itemStackDepth;
    char lastAxis;
    char lastTimer;
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

int menuGetInputDir(char axis, char* lastAxis, char* lastTimer);

#endif