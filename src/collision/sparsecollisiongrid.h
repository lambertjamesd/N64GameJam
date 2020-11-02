
#ifndef _COLLISION_SPARSECOLLISOIN_H
#define _COLLISION_SPARSECOLLISOIN_H

#include "collisiondata.h"

#define MAX_CELL_NODES  256

struct SparseCollisionGridCell {
    struct SparseCollisionGridCell* next;
    struct CollisionTransformedCollider* collider;
};

struct SparseCollisionGrid {
    // cells[(x * height + y) & cellCountModMask]
    struct SparseCollisionGridCell** cells;
    int height;
    int cellCountModMask;
    struct SparseCollisionGridCell unusedCells[MAX_CELL_NODES];
    struct SparseCollisionGridCell* nextUnusedCell;
};

void sparseCollisionReindex(struct SparseCollisionGrid* to, struct CollisionTransformedCollider* collider, struct CollisionBox* bb, struct CollisionBox* prevBB);

#endif