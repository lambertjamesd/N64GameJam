
#ifndef _SAVE_SAVEFILE_H
#define _SAVE_SAVEFILE_H

#define MAX_LEVELS  64

#define SAVEFILE_LEARNED_MOVE           (1 << 0)
#define SAVEFILE_LEARNED_JUMP           (1 << 1)
#define SAVEFILE_LEARNED_FOUND_ROBOT    (1 << 2)
#define SAVEFILE_LEARNED_ATTACK         (1 << 3)
#define SAVEFILE_LEARNED_SWITCH         (1 << 4)

struct SaveFile {
    int tutorialFlags;
    int levelData[MAX_LEVELS];
};

extern struct SaveFile gSaveFile;

#endif