
#ifndef _CUTSCENE_CUTSCENE_H
#define _CUTSCENE_CUTSCENE_H

#include "src/audio/audio.h"

struct CutsceneFrame {
    float duration;
    int slideIndex;
    float yVelocity;
    float yOffset;
};

enum CutsceneEventType {
    CutsceneEventTypeSeq,
};

struct CutsceneEvent {
    float at;
    enum CutsceneEventType eventType;
    union {
        struct SeqPlayEvent seq;
    };
};

struct Cutscene {
    struct CutsceneFrame* frames;
    int frameCount;
    struct CutsceneEvent* events;
    int eventCount;
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