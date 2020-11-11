
#ifndef _AUDIO_PLAYER_SOUNDS
#define _AUDIO_PLAYER_SOUNDS

#include "soundarray.h"

enum PlayerSounds {
    PlayerSoundsJump,
    PlayerSoundsJumpPeak,
    PlayerSoundsLand,
    PlayerSoundsCount,
};

extern struct SoundArray* gPlayerSoundArray;
extern ALSndId gPlayerSoundIds[PlayerSoundsCount];

void playerSoundsInit();

#endif