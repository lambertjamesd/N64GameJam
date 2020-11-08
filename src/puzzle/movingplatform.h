
#ifndef _PUZZLE_MOVING_PLATFORM_H
#define _PUZZLE_MOVING_PLATFORM_H

#include "src/math/basictransform.h"
#include "src/collision/sparsecollisiongrid.h"
#include "src/time/time.h"
#include "signal.h"

#define PLATFORM_MOVE_SPEED 2.0f

struct MovingPlatform;

struct MovingPlatformSlot {
    // right, down, left, up
    struct MovingPlatformSlot* adjacent[4];
    struct MovingPlatform* occupant;
    struct Vector3 position;
};

struct MovingPlatform {
    struct BasicTransform transform;
    struct CollisionTransformedCollider collider;
    struct TimeUpdateListener updateListener;
    struct MovingPlatformSlot* currentSlot;
    struct CollisionBox prevBB;
    int moveDirection;
    int nextMoveDir;
    int signalIndex;
    int lastSignalValue;
};

void movingPlatformJoinSlots(struct MovingPlatformSlot* platform, int count);
void movingPlatformSlotInit(struct MovingPlatformSlot* platform, struct Vector3* position);
void movingPlatformInit(struct MovingPlatform* platform, struct Vector3* position, struct MovingPlatformSlot* slot, int color);

#endif