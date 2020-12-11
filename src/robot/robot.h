
#ifndef _ROBOT_ROBOT_H
#define _ROBOT_ROBOT_H

#include "src/collision/sphereactor.h"
#include "src/math/basictransform.h"
#include "src/time/time.h"
#include "src/math/vector2.h"
#include "src/effects/shadow.h"
#include "src/effects/teleport.h"
#include "src/effects/explosion.h"
#include "src/effects/respawn.h"

#define ROBOT_ACCEL 16.0f
#define ROBOT_SPEED 3.0f

#define ROBOT_RADIUS 0.8f
#define ROBOT_TURN_RATE (M_PI * 1.5f)

#define ROBOT_BB_RADIUS 0.9f

#define ROBOT_ATTACK_DISTANCE   1.4f
#define ROBOT_ATTACK_RADIUS     0.25f
#define ROBOT_ATTACK_DELAY      1.0f

#define ROBOT_MAX_EXPLOSIONS    2

#define ROBOT_IS_CUTSCENE  SPHERE_ACTOR_FLAG_1
#define ROBOT_IS_INVISIBLE  SPHERE_ACTOR_FLAG_2

struct Robot;

typedef void (*RobotState)(struct Robot* robot);

struct Robot {
    struct BasicTransform transform;
    struct SphereActor actor;
    RobotState state;
    struct TimeUpdateListener updateListener;
    struct Vector2 rotation;
    struct CollisionTransformedCollider collider;
    struct Explosion explosions[ROBOT_MAX_EXPLOSIONS];
    struct RespawnEffect respawn;
    float attackTimer;
    // state sepecific
    struct DropShadow shadow;
    struct TeleportEffect teleport;
    short nextExplosion;
    short controllerIndex;
};

extern struct Robot gRobot;

void robotRender(struct DynamicActor* data, struct GraphicsState* state);
void robotInit();
void robotReset(struct Vector3* startLocation);
void robotFinishLevel(struct Robot* cadet);

#endif