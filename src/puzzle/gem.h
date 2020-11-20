
#ifndef _PUZZLE_GEM_H
#define _PUZZLE_GEM_H

#include "src/math/basictransform.h"
#include "src/collision/sparsecollisiongrid.h"
#include "src/time/time.h"

#define GEM_FLAGS_PREVIOUSLY_COLLECTED  0x1
#define GEM_FLAGS_COLLECT_ANIM          0x2
#define GEM_FLAGS_COLLECTED             0x4

struct Gem {
    struct BasicTransform transform;
    struct CollisionTransformedCollider collider;
    struct TimeUpdateListener updateListener;
    short flags;
    short index;
    float animationTimer;
};

void gemInit(struct Gem* gem, struct Vector3* pos, short index);

extern Gfx _gem_mat[];
extern Gfx _gem_mesh_tri_0[];

#endif