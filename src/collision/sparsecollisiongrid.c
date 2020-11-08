
#include "sparsecollisiongrid.h"

#include "src/input/controller.h"

#include <math.h>

#define CELL_INDEX(x, z) ((((x) << SPARSE_GRID_HEIGHT_SHIFT) + (z)) & SPARSE_GRID_CELL_COUNT_MASK)
#define MIN_INDEX(val) ((int)floorf((val) * 0.5f + 0.5f))
#define MAX_INDEX(val) ((int)ceilf((val) * 0.5f + 0.5f))

void sparseCollisionInit(struct SparseCollisionGrid* grid) {
    int i;

    for (i = 0; i < SPARSE_GRID_CELL_COUNT; ++i) {
        grid->cells[i] = 0;
    }

    grid->nextUnusedCell = &grid->unusedCells[0];

    for (i = 0; i < MAX_CELL_NODES; ++i) {
        grid->unusedCells[i].collider = 0;
        if (i + 1< MAX_CELL_NODES) {
            grid->unusedCells[i].next = &grid->unusedCells[i + 1];
        } else {
            grid->unusedCells[i].next = 0;
        }
    }
}

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
        int minX = MIN_INDEX(prevBB->min.x);
        int minZ = MIN_INDEX(prevBB->min.z);

        int maxX = MAX_INDEX(prevBB->max.x);
        int maxZ = MAX_INDEX(prevBB->max.z);

        int x;
        int z;

        for (x = minX; x < maxX; ++x) {
            for (z = minZ; z < maxZ; ++z) {
                sparseCollisionRemoveIndex(to, CELL_INDEX(x, z), collider);
            }
        }
    }

    if (bb) {
        int minX = MIN_INDEX(bb->min.x);
        int minZ = MIN_INDEX(bb->min.z);

        int maxX = MAX_INDEX(bb->max.x);
        int maxZ = MAX_INDEX(bb->max.z);

        int x;
        int z;

        for (x = minX; x < maxX; ++x) {
            for (z = minZ; z < maxZ; ++z) {
                sparseCollisionAddIndex(to, CELL_INDEX(x, z), collider);
            }
        }
    }
}

int collisionSparseGridCellCollideSphere(struct Vector3* center, float radius, struct SparseCollisionGridCell* cell, int collisionMask, struct CollisionResult* result, int resultCountCheck) {
    int didCollide = 0;

    while (cell) {
        // next cell retrieved here in case trigger callbacks remove current collider
        struct SparseCollisionGridCell* nextCell = cell->next;
        int alreadyChecked = 0;
        int i;

        for (i = resultCountCheck; i < result->contactCount; i = i + 1) {
            if (result->contacts[i].transform == cell->collider->transform) {
                alreadyChecked = 1;
                break;
            }
        }

        if (!alreadyChecked) {
            int startCount = result->contactCount;

            if (collisionTransColliderCollideSphere(center, radius, cell->collider, collisionMask, result)) {
                didCollide = 1;

                for (i = startCount; i < result->contactCount; ++i) {
                    result->contacts[i].transform = cell->collider->transform;
                }

                if (result->contactCount == MAX_CONTACT_POINTS) {
                    return 1;
                }
            }
        }

        cell = nextCell;
    }

    return didCollide;
}

int collisionSparseGridCollideSphere(struct Vector3* center, float radius, struct SparseCollisionGrid* grid, int collisionMask, struct CollisionResult* result) {
    int didCollide = 0;

    int minX = MIN_INDEX(center->x - radius);
    int minZ = MIN_INDEX(center->z - radius);

    int maxX = MAX_INDEX(center->x + radius);
    int maxZ = MAX_INDEX(center->z + radius);

    int x;
    int z;

    int startCount = result->contactCount;

    for (x = minX; x < maxX; ++x) {
        for (z = minZ; z < maxZ; ++z) {
            if (collisionSparseGridCellCollideSphere(center, radius, grid->cells[CELL_INDEX(x, z)], collisionMask, result, startCount)) {
                didCollide = 1;

                if (result->contactCount == MAX_CONTACT_POINTS) {
                    return 1;
                }
            }
        }
    }

    return didCollide;
}

float collisionSparseGridCellRaycast(struct SparseCollisionGridCell* cell, struct Vector3* position, struct Vector3* dir, int collisionMask, float maxDistance, struct ContactPoint* hit) {
    float result = RAYCAST_NO_HIT;

    while (cell) {
        struct ContactPoint hitCheck;
        float hitDistance = collisionTransColliderRaycast(cell->collider, position, dir, collisionMask, &hitCheck);

        if (hitDistance < result) {
            result = hitDistance;
            *hit = hitCheck;
        }

        cell = cell->next;
    }

    return result;
}

float collisionSparseGridRaycast(struct SparseCollisionGrid* grid, struct Vector3* position, struct Vector3* dir, int collisionMask, float maxDistance, struct ContactPoint* hit) {
    int x = MIN_INDEX(position->x);
    int z = MIN_INDEX(position->z);

    return collisionSparseGridCellRaycast(grid->cells[CELL_INDEX(x, z)], position, dir, collisionMask, maxDistance, hit);
}