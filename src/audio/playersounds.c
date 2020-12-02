
#include "playersounds.h"
#include "src/boot.h"
#include "audio.h"

struct SoundArray* gPlayerSoundArray;
ALSndId gPlayerSoundIds[PlayerSoundsCount];

extern char _player_soundsSegmentRomEnd[], _player_soundsSegmentRomStart[];
extern char _player_sounds_tableSegmentRomStart[];

extern char _intro_cutsceneSegmentRomEnd[], _intro_cutsceneSegmentRomStart[];
extern char _intro_cutscene_tableSegmentRomStart[];

static enum SoundsBanks gCurrentBank = SoundBankNone;

struct AudioBank {
    char* romStart;
    char* romEnd;
    char* tableStart;
    int soundCount;
};

struct AudioBank gAudioBanks[] = {
    {_player_soundsSegmentRomStart, _player_soundsSegmentRomEnd, _player_sounds_tableSegmentRomStart, PlayerSoundsCount},
    {_intro_cutsceneSegmentRomStart, _intro_cutsceneSegmentRomEnd, _intro_cutscene_tableSegmentRomStart, IntroSoundCount},
};


void playerSoundsUseBank(enum SoundsBanks bankIndex) {
    if (bankIndex == gCurrentBank) {
        return;
    }

    int i;
    if (gCurrentBank != -1) {
        for (i = 0; i < gAudioBanks[gCurrentBank].soundCount; ++i) {
            alSndpDeallocate(&gSoundPlayer, gPlayerSoundIds[i]);
            gPlayerSoundIds[i] = -1;
        }
    }

    gCurrentBank = bankIndex;

    struct AudioBank* bank = &gAudioBanks[bankIndex];

    u32 bankLen = bank->romEnd - bank->romStart;

    romCopy(bank->romStart, (char *)gPlayerSoundArray, bankLen);
    
    soundArrayInit(gPlayerSoundArray, (u8 *) bank->tableStart);

    for (i = 0; i < PlayerSoundsCount && i < gPlayerSoundArray->soundCount && i < bank->soundCount; ++i) {
        gPlayerSoundIds[i] = alSndpAllocate(&gSoundPlayer, gPlayerSoundArray->sounds[i]);
    }

    for (; i < PlayerSoundsCount; ++i) {
        gPlayerSoundIds[i] = -1;
    }
}

void playerSoundsInit() {
    int i;
    int maxSize = 0;

    for (i = 0; i < sizeof(gAudioBanks)/sizeof(*gAudioBanks); ++i) {
        int currSize = gAudioBanks[i].romEnd - gAudioBanks[i].romStart;

        if (currSize > maxSize) {
            maxSize = currSize;
        }
    }
    gPlayerSoundArray = alHeapAlloc(&gAudioHeap, 1, maxSize);
}