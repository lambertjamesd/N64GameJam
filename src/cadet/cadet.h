
#ifndef _CADET_CADET_C
#define _CADET_CADET_C

#include "src/math/basictransform.h"
#include "src/time/time.h"

#define CADET_RADIUS    0.2f
#define CADET_SPEED     4.0f
#define CADET_ACCEL     14.0f
#define CADET_AIR_ACCEL 4.0f

#define CADET_JUMP_IMPULSE  4.0f
#define CADET_JUMP_ACCEL    4.0f

struct Cadet;

typedef void (*CadetState)(struct Cadet* cadet);

#define CADET_IS_JUMPING  (1 << 0)
#define CADET_IS_GROUNDED  (1 << 1)

struct Cadet {
    struct BasicTransform transform;
    CadetState state;
    struct TimeUpdateListener updateListener;
    struct Vector3 velocity;
    struct Vector3 lastStableLocation;
    int stateFlags;

    struct BasicTransform* anchor;
    struct Vector3 relativeToAnchor;
};

extern struct Cadet gCadet;

void cadetInit();
void cadetReset(struct Vector3* startLocation);

#endif