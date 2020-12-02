
#ifndef _CUTSCENE_CUTSCENE_H
#define _CUTSCENE_CUTSCENE_H

#include "src/audio/audio.h"
#include "src/audio/playersounds.h"

struct CutsceneFrame {
    float duration;
    int slideIndex;
    float yVelocity;
    float yOffset;
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