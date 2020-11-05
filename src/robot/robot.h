
#ifndef _ROBOT_ROBOT_H
#define _ROBOT_ROBOT_H

#include "src/math/basictransform.h"
#include "src/time/time.h"

struct Robot;

typedef void (*RobotState)(struct Robot* robot);

struct Robot {
    struct BasicTransform transform;

};

#endif