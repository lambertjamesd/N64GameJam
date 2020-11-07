
#ifndef _PUZZLE_DOOR_H
#define _PUZZLE_DOOR_H

#include "src/math/basictransform.h"
#include "src/collision/sparsecollisiongrid.h"
#include "src/time/time.h"
#include "signal.h"

struct PuzzleDoor {
    struct BasicTransform transform;
    struct CollisionTransformedCollider collider;
    struct TimeUpdateListener updateListener;
    int signalIndex;
    struct Vector3 closedPosition;
};

void doorInit(struct PuzzleDoor* door, struct Vector3* position, int color);

#endif