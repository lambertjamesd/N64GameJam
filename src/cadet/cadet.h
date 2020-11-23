
#ifndef _CADET_CADET_C
#define _CADET_CADET_C

#include "src/collision/sphereactor.h"
#include "src/effects/shadow.h"
#include "src/effects/teleport.h"
#include "src/effects/walkanim.h"
#include "src/math/basictransform.h"
#include "src/math/vector2.h"
#include "src/time/time.h"

#define CADET_RADIUS        0.2f
#define CADET_SPEED         5.6f
#define CADET_AIR_SPEED     4.48f
#define CADET_BACKACCEL     24.0f
#define CADET_ACCEL         12.0f
#define CADET_AIR_ACCEL     2.0f
#define CADET_AIR_BACKACCEL 8.0f

#define CADET_HORZ_IMPULSE  1.2f
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
    struct WalkAnim walkAnim;
    // state sepecific
    float coyoteTimer;
    struct TeleportEffect teleport;
    short footstepSound;
};

extern struct Cadet gCadet;

void cadetInit();
void cadetReset(struct Vector3* startLocation);
void cadetFinishLevel(struct Cadet* cadet);

#endif