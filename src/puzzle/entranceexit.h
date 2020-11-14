
#ifndef _PUZZLE_ENTRANCEEXIT_H
#define _PUZZLE_ENTRANCEEXIT_H

#include "src/math/basictransform.h"
#include "src/collision/sparsecollisiongrid.h"
#include "src/time/time.h"

struct EntranceExit {
    struct BasicTransform transform;
    struct CollisionTransformedCollider collider;
    struct CollisionTransformedCollider exitTrigger;
    struct TimeUpdateListener updateListener;
    int isActive;
};

void entranceExitInit(struct EntranceExit* exit, struct Vector3* pos, int isCadet);

extern struct EntranceExit gCadetExit;
extern struct EntranceExit gRobotExit;

#endif