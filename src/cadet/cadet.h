
#ifndef _CADET_CADET_C
#define _CADET_CADET_C

#include "src/collision/sphereactor.h"
#include "src/math/basictransform.h"
#include "src/time/time.h"
#include "src/math/vector2.h"
#include "src/effects/shadow.h"

#define CADET_RADIUS    0.2f
#define CADET_SPEED     2.5f
#define CADET_ACCEL     12.0f
#define CADET_AIR_ACCEL 3.0f

#define CADET_JUMP_IMPULSE  4.0f
#define CADET_JUMP_ACCEL    4.0f

struct Cadet;

typedef void (*CadetState)(struct Cadet* cadet);

#define CADET_IS_JUMPING  (1 << 1)

struct Cadet {
    struct BasicTransform transform;
    struct SphereActor actor;
    CadetState state;
    struct TimeUpdateListener updateListener;
    struct Vector2 rotation;
    float coyoteTimer;
    struct DropShadow shadow;
};

extern struct Cadet gCadet;

void cadetInit();
void cadetReset(struct Vector3* startLocation);

#endif