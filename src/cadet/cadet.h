
#ifndef _CADET_CADET_C
#define _CADET_CADET_C

#include "src/math/basictransform.h"
#include "src/time/time.h"

struct Cadet;

typedef void (*CadetState)(struct Cadet* cadet);

struct Cadet {
    struct BasicTransform transform;
    CadetState state;
    struct TimeUpdateListener updateListener;
    struct Vector3 velocity;
    struct Vector3 lastStableLocation;
    int isJumping;

    struct BasicTransform* anchor;
    struct Vector3 relativeToAnchor;
};

extern struct Cadet gCadet;

void cadetInit();
void cadetReset(struct Vector3* startLocation);

#endif