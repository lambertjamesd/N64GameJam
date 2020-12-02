
#ifndef _AUDIO_PLAYER_SOUNDS
#define _AUDIO_PLAYER_SOUNDS

#include "soundarray.h"

enum SoundsBanks {
    SoundBankLevel,
};

enum PlayerSounds {
    PlayerSoundsJump,
    PlayerSoundsJumpPeak,
    PlayerSoundsLand,
    PlayerSoundsFootsteps,
    PlayerSoundsSlide1,
    PlayerSoundsSlide2,
    PlayerSoundsCadetWarp,
    PlayerSoundsRobotWarp,
    PlayerSoundsBothWarp,
    PlayerCadetFall,
    PlayerRobotFall,
    PlayerSoundsLandMetal,
    PlayerSoundsFootstepsMetal,
    PlayerRobotAttack,
    PlayerRobotMove,
    PlayerSwitchBack,
    PlayerSwitch,
    ButtonSoundsBig,
    ButtonSoundsSmall,
    GoalTouchBig,
    GoalTouchSmall,
    SoundUIScroll,
    SoundUISelect,
    SoundGemCollect,
    PlayerSoundsCount,
};

extern struct SoundArray* gPlayerSoundArray;
extern ALSndId gPlayerSoundIds[PlayerSoundsCount];

void playerSoundsUseBank(enum SoundsBanks bankIndex);
void playerSoundsInit();

#endif