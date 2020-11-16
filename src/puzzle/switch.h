
#ifndef _PUZZLE_SWITCH_H
#define _PUZZLE_SWITCH_H

#include "src/math/basictransform.h"
#include "src/collision/sparsecollisiongrid.h"
#include "src/time/time.h"
#include "signal.h"

enum PuzzleSwitchType {
    PuzzleSwitchTypeLarge,
    PuzzleSwitchTypeSmall,
    PuzzleSwitchTypeLargePermanant,
    PuzzleSwitchTypeSmallPermanant,
};

#define PUZZLE_SWITCH_SIZE      1
#define PUZZLE_SWITCH_PERMANANT 2

struct PuzzleSwitch {
    struct BasicTransform transform;
    struct CollisionTransformedCollider collider;
    struct SignalSender sender;
    struct TimeUpdateListener updateListener;
    short didTrigger;
    short switchType;
};

void switchInit(struct PuzzleSwitch* puzzleSwitch, struct Vector3* position, enum PuzzleSwitchType type, int color);

#endif