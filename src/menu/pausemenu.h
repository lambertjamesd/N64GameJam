
#ifndef _MENU_PAUSE_MENU
#define _MENU_PAUSE_MENU

#include "src/time/time.h"

enum PauseMenuItem {
    PauseMenuItemResume,
    PauseMenuItemRestart,
    PauseMenuItemSoundVolume,
    PauseMenuItemMusicVolume,
    PauseMenuItemSave,
    PauseMenuItemMainMenu,
    PauseMenuItemCount,
};

struct PauseMenu {
    int selectedItem;
    struct TimeUpdateListener updateListener;
    char lastVerticalInput;
};

extern struct PauseMenu gPauseMenu;

void pauseMenuShow(struct PauseMenu* pauseMenu);

#endif