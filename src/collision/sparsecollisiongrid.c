
#include "sparsecollisiongrid.h"

#include <math.h>

#define CELL_INDEX(grid, x, z) ((x * grid->height + z) & grid->cellCountModMask)

void sparseCollisionRemoveIndex(struct SparseCollisionGrid* to, int cellIndex, struct CollisionTransformedCollider* collider) {
    struct SparseCollisionGridCell* prev = 0;
    struct SparseCollisionGridCell* curr = to->cells[cellIndex];

    while (curr) {
        if (curr->collider == collider) {
            if (prev) {
                prev->next = curr->next;
            } else {
                to->cells[cellIndex] = curr->next;
            }
    
            curr->next = to->nextUnusedCell;
            to->nextUnusedCell = curr;
            return;
        }

        prev = curr;
        curr = curr->next;
    }
}

void sparseCollisionAddIndex(struct SparseCollisionGrid* to, int cellIndex, struct CollisionTransformedCollider* collider) {
    struct SparseCollisionGridCell* cell = to->nextUnusedCell;

    if (cell) {
        to->nextUnusedCell = cell->next;

        cell->next = to->cells[cellIndex];
        to->cells[cellIndex] = cell;
        cell->collider = collider;
    }
}

void sparseCollisionReindex(struct SparseCollisionGrid* to, struct CollisionTransformedCollider* collider, struct CollisionBox* bb, struct CollisionBox* prevBB) {
    if (prevBB) {
        int minX = (int)floorf(prevBB->min.x * 0.5f + 0.5f);
        int minZ = (int)floorf(prevBB->min.z * 0.5f + 0.5f);

        int maxX = (int)ceilf(prevBB->max.x * 0.5f + 0.5f);
        int maxZ = (int)ceilf(prevBB->max.z * 0.5f + 0.5f);

        int x;
        int z;

        for (x = minX; x < maxX; ++x) {
            for (z = minZ; z < maxZ; ++z) {
                sparseCollisionRemoveIndex(to, CELL_INDEX(to, x, z), collider);
            }
        }
    }

    if (bb) {
        int minX = (int)floorf(bb->min.x * 0.5f + 0.5f);
        int minZ = (int)floorf(bb->min.z * 0.5f + 0.5f);

        int maxX = (int)ceilf(bb->max.x * 0.5f + 0.5f);
        int maxZ = (int)ceilf(bb->max.z * 0.5f + 0.5f);

        int x;
        int z;

        for (x = minX; x < maxX; ++x) {
            for (z = minZ; z < maxZ; ++z) {
                sparseCollisionAddIndex(to, CELL_INDEX(to, x, z), collider);
            }
        }
    }
}