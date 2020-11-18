
#ifndef _AUDIO_PLAYER_SOUNDS
#define _AUDIO_PLAYER_SOUNDS

#include "soundarray.h"

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
    PlayerSoundsCount,
};

extern struct SoundArray* gPlayerSoundArray;
extern ALSndId gPlayerSoundIds[PlayerSoundsCount];

void playerSoundsInit();

#endif