
#include "savefile.h"

struct SaveFile gSaveFile;


int saveFileDidCollectGem(int level, int gemIndex) {
    return (gSaveFile.levelData[level] & (SAVEFILE_LEVEL_GEMS << gemIndex)) != 0;
}

void saveFileMarkCollectedGem(int level, int gemIndex) {
    gSaveFile.levelData[level] |= SAVEFILE_LEVEL_GEMS << gemIndex;
}