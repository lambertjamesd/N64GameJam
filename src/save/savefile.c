
#include "savefile.h"

struct SaveFile gSaveFile;


int saveFileDidCollectGem(int level, int gemIndex) {
    return (gSaveFile.levelData[level] & (SAVEFILE_LEVEL_GEMS << gemIndex)) != 0;
}

void saveFileMarkCollectedGem(int level, int gemIndex) {
    gSaveFile.levelData[level] |= SAVEFILE_LEVEL_GEMS << gemIndex;
}

int saveFileIsLevelComplete(int level) {
    return gSaveFile.levelData[level] & SAVEFILE_LEVEL_BEAT;
}

void saveFileMarkDidCompleteLevel(int level) {
    gSaveFile.levelData[level] |= SAVEFILE_LEVEL_BEAT;
}

void saveFileErase() {
    gSaveFile.header = SAVEFILE_HEADER;
    gSaveFile.tutorialFlags = 0;
    int i;
    for (i = 0; i < MAX_LEVELS; ++i) {
        gSaveFile.levelData[i] = 0;
    }
}