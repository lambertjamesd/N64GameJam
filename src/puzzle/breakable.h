
#ifndef _PUZZLE_BREAKABLE_H
#define _PUZZLE_BREAKABLE_H

#include "src/math/basictransform.h"
#include "src/collision/sparsecollisiongrid.h"
#include "src/graphics/dynamic.h"

enum BreakableType {
    BreakableTypePlatform,
    BreakableTypeBarrier,
};

struct Breakable {
    struct BasicTransform transform;
    struct CollisionTransformedCollider collider;
    int type;
    ActorId renderActorId;
};

void breakableInit(struct Breakable* door, struct Vector3* position, int type);

#endif