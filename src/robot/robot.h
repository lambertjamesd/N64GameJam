
#ifndef _ROBOT_ROBOT_H
#define _ROBOT_ROBOT_H

#include "src/collision/sphereactor.h"
#include "src/math/basictransform.h"
#include "src/time/time.h"
#include "src/math/vector2.h"

#define ROBOT_ACCEL 8.0f
#define ROBOT_SPEED 2.0f

#define ROBOT_RADIUS 0.8f
#define ROBOT_TURN_RATE (M_PI)

struct Robot;

typedef void (*RobotState)(struct Robot* robot);

struct Robot {
    struct BasicTransform transform;
    struct SphereActor actor;
    RobotState state;
    struct TimeUpdateListener updateListener;
    struct Vector2 rotation;
};

extern struct Robot gRobot;

void robotInit();
void robotReset(struct Vector3* startLocation);

#endif