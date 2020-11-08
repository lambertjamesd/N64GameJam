
#ifndef _COLLISION_SPARSECOLLISOIN_H
#define _COLLISION_SPARSECOLLISOIN_H

#include "collisiondata.h"

#define MAX_CELL_NODES  256
// Must be a power of 2
#define SPARSE_GRID_CELL_COUNT      16
// 2^n - 1
#define SPARSE_GRID_CELL_COUNT_MASK   (SPARSE_GRID_CELL_COUNT - 1)
// 4 == 1 << 2
#define SPARSE_GRID_HEIGHT_SHIFT      2

struct SparseCollisionGridCell {
    struct SparseCollisionGridCell* next;
    struct CollisionTransformedCollider* collider;
};

struct SparseCollisionGrid {
    // cells[(x * height + y) & cellCountModMask]
    struct SparseCollisionGridCell* cells[SPARSE_GRID_CELL_COUNT];
    struct SparseCollisionGridCell unusedCells[MAX_CELL_NODES];
    struct SparseCollisionGridCell* nextUnusedCell;
};

void sparseCollisionInit(struct SparseCollisionGrid* grid);
void sparseCollisionReindex(struct SparseCollisionGrid* to, struct CollisionTransformedCollider* collider, struct CollisionBox* bb, struct CollisionBox* prevBB);

int collisionSparseGridCollideSphere(struct Vector3* center, float radius, struct SparseCollisionGrid* grid, int collisionMask, struct CollisionResult* result);
float collisionSparseGridRaycast(struct SparseCollisionGrid* grid, struct Vector3* position, struct Vector3* dir, int collisionMask, float maxDistance, struct ContactPoint* hit);

#endif