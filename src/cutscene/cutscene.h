
#ifndef _CUTSCENE_CUTSCENE_H
#define _CUTSCENE_CUTSCENE_H

struct CutsceneFrame {
    float duration;
    int slideIndex;
    float yVelocity;
    float yOffset;
};

struct Cutscene {
    struct CutsceneFrame* frames;
    int frameCount;
};

struct CutscenePlayer {
    struct Cutscene* cutscene;
    int currentFrame;
    float currentTime;
    int targetLevel;
};

void cutScenePlay(struct Cutscene* cutscene, int nextLevel);

#endif