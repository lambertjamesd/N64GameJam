
#include "playersounds.h"
#include "src/boot.h"
#include "audio.h"

struct SoundArray* gPlayerSoundArray;
ALSndId gPlayerSoundIds[PlayerSoundsCount];

extern char _player_soundsSegmentRomEnd[], _player_soundsSegmentRomStart[];
extern char _player_sounds_tableSegmentRomStart[];

void playerSoundsInit() {
    int i;

    u32 bankLen = _player_soundsSegmentRomEnd - _player_soundsSegmentRomStart;
    gPlayerSoundArray = alHeapAlloc(&gAudioHeap, 1, bankLen);
    romCopy(_player_soundsSegmentRomStart, (char *)gPlayerSoundArray, bankLen);
    
    soundArrayInit(gPlayerSoundArray, (u8 *) _player_sounds_tableSegmentRomStart);

    for (i = 0; i < PlayerSoundsCount && i < gPlayerSoundArray->soundCount; ++i) {
        gPlayerSoundIds[i] = alSndpAllocate(&gSoundPlayer, gPlayerSoundArray->sounds[i]);
    }

    for (; i < PlayerSoundsCount; ++i) {
        gPlayerSoundIds[i] = -1;
    }
}