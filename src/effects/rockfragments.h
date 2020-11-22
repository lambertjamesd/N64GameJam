
#ifndef EFFECTS_ROCK_FRAGMENTS_H
#define EFFECTS_ROCK_FRAGMENTS_H

#include "src/math/basictransform.h"
#include "src/time/time.h"

#define MAX_FRAGMENT_COUNT  3

struct RockFragments {
    struct TimeUpdateListener updateListener;
    struct BasicTransform transforms[MAX_FRAGMENT_COUNT];
    struct Vector3 velocity[MAX_FRAGMENT_COUNT];
    struct Quaternion angularVelocity[MAX_FRAGMENT_COUNT];
    float timer;
    int fragmentCount;
    int renderActorId;
};

void rockFragmentsInit(struct RockFragments* fragments, struct Vector3* stackPos, int fragmentCount, struct Vector3* explosionOrigin);

#endif