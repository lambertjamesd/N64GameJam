
#ifndef _SAVE_SAVEFILE_H
#define _SAVE_SAVEFILE_H

#define MAX_LEVELS  64

#define SAVEFILE_LEARNED_MOVE           (1 << 0)
#define SAVEFILE_LEARNED_JUMP           (1 << 1)
#define SAVEFILE_LEARNED_FOUND_ROBOT    (1 << 2)
#define SAVEFILE_LEARNED_ATTACK         (1 << 3)
#define SAVEFILE_LEARNED_SWITCH         (1 << 4)

#define SAVEFILE_HEADER     0xBEEF

#define SAVEFILE_LEVEL_BEAT     0x1
#define SAVEFILE_LEVEL_GEMS     0x2

struct SaveFile {
    int header;
    int tutorialFlags;
    int levelData[MAX_LEVELS];
};

extern struct SaveFile gSaveFile;

int saveFileDidCollectGem(int level, int gemIndex);
void saveFileMarkCollectedGem(int level, int gemIndex);

int saveFileIsLevelComplete(int level);
void saveFileMarkDidCompleteLevel(int level);
void saveFileErase();

#endif