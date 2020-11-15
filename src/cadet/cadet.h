
#ifndef _CADET_CADET_C
#define _CADET_CADET_C

#include "src/collision/sphereactor.h"
#include "src/math/basictransform.h"
#include "src/time/time.h"
#include "src/math/vector2.h"
#include "src/effects/shadow.h"
#include "src/effects/teleport.h"

#define CADET_RADIUS    0.2f
#define CADET_SPEED     3.5f
#define CADET_AIR_SPEED 2.8f
#define CADET_ACCEL     12.0f
#define CADET_AIR_ACCEL 2.0f

#define CADET_JUMP_IMPULSE  5.5f
#define CADET_JUMP_ACCEL    8.0f

struct Cadet;

typedef void (*CadetState)(struct Cadet* cadet);

#define CADET_IS_JUMPING  SPHERE_ACTOR_FLAG_0
#define CADET_IS_CUTSCENE  SPHERE_ACTOR_FLAG_1
#define CADET_IS_INVISIBLE  SPHERE_ACTOR_FLAG_2

struct Cadet {
    struct BasicTransform transform;
    struct SphereActor actor;
    CadetState state;
    struct TimeUpdateListener updateListener;
    struct Vector2 rotation;
    struct DropShadow shadow;
    float accumTime;
    float gravity;
    // state sepecific
    float coyoteTimer;
    struct TeleportEffect teleport;
};

extern struct Cadet gCadet;

void cadetInit();
void cadetReset(struct Vector3* startLocation);
void cadetFinishLevel(struct Cadet* cadet);

#endif