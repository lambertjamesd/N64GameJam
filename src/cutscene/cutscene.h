
#ifndef _CUTSCENE_CUTSCENE_H
#define _CUTSCENE_CUTSCENE_H

#include "src/audio/audio.h"
#include "src/audio/playersounds.h"

enum CutsceneFrames {
    _00_rocket,
	_01_insidecockpit,
	_01_insidecockpit_flash,
	_02_fuelgauge,
	_02_fuelgauge_flash,
	_03_mapcloseup,
	_04_seebutton,
	_05_pushbutton,
	_06_mapchanged,
	_08_rocket_turn,
	_09_see_planet,
	_00_bad_ending,
	_00_bad_ending_closeup,
	_01_see_robot,
	_02_sad_cadet,
	_03_leave_robot,
	_04_leave_planet,
	_00_good_ending,
	_00_good_ending_closeup,
    _02_leave_with_robot,
	_10_credits_text,
	_11_homebrew,
};

struct CutsceneFrame {
    float duration;
    int slideIndex;
    float yVelocity;
    float yOffset;
    float dxVelocity;
    float dxOffset;
};

enum CutsceneEventType {
    CutsceneEventTypeSeq,
    CutsceneEventTypeSeqStop,
    CutsceneEventTypeSound,
    CutsceneEventTypeSoundStop,
};

struct CutsceneEvent {
    float at;
    enum CutsceneEventType eventType;
    union {
        struct SeqPlayEvent seq;
        struct  {
            int soundId;
            float pitch;
            float volume;
            float pan;
            int priority;
        } sound;
    };
};

struct Cutscene {
    struct CutsceneFrame* frames;
    int frameCount;
    struct CutsceneEvent* events;
    int eventCount;
    enum SoundsBanks soundBank;
};

struct CutscenePlayer {
    struct Cutscene* cutscene;
    int currentFrame;
    int nextEvent;
    float currentTime;
    float totalTime;
    int targetLevel;
};

void cutScenePlay(struct Cutscene* cutscene, int nextLevel);

#endif