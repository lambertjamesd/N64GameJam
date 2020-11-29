
#ifndef _SAVE_SAVEFILE_H
#define _SAVE_SAVEFILE_H

#define MAX_LEVELS  27

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
    char tutorialFlags;
    char levelData[MAX_LEVELS];
};

int saveFileDidCollectGem(int level, int gemIndex);
void saveFileMarkCollectedGem(int level, int gemIndex);

int saveFileCalculateGemsCollected(int maxLevels);

void saveFileLoad();
void saveFileSave();

int saveFileIsLevelComplete(int level);
void saveFileMarkDidCompleteLevel(int level);
void saveFileErase();
void saveUnlockAll();

int saveFileCheckTutorial(int flags);
void saveFileMarkTutorial(int flags);

int saveFileNeedsSave();

#endif