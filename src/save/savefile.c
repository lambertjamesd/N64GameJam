
#include "savefile.h"

#include <ultra64.h>

struct SaveFile gSaveFile;
extern OSMesgQueue gContMessageQ;
int gEepromProbe;

void saveFileNew() {
    gSaveFile.header = SAVEFILE_HEADER;
    gSaveFile.tutorialFlags = 0;
    int i;
    for (i = 0; i < MAX_LEVELS; ++i) {
        gSaveFile.levelData[i] = 0;
    }
}

void saveFileLoad() {
    gEepromProbe = osEepromProbe(&gContMessageQ);
    if (gEepromProbe) {
        osEepromLongRead(&gContMessageQ, 0, (char*)&gSaveFile, sizeof(struct SaveFile));

        if (gSaveFile.header != SAVEFILE_HEADER) {
            saveFileNew();
        }
    } else {
        saveFileNew();
    }
}

void saveFileSave() {
    if (gEepromProbe) {
        OSMesg dummy;
        (void)osRecvMesg(&gContMessageQ, &dummy, OS_MESG_BLOCK);
        osEepromLongWrite(&gContMessageQ, 0, (char*)&gSaveFile, sizeof(struct SaveFile));
        osContStartReadData(&gContMessageQ);
    }
}

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
    saveFileNew();
    saveFileSave();
}