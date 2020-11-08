
#include "levelcollisiongrid.h"
#include <ultra64.h>

#include "src/input/controller.h"

typedef void (*VertexTransform)(struct Vector3* vertex);

void VertexRotateNone(struct Vector3* vertex) {
    
}

void VertexRotate90(struct Vector3* vertex) {
    float tmp = vertex->x;
    vertex->x = -vertex->z;
    vertex->z = tmp;
}

void VertexRotate180(struct Vector3* vertex) {
    vertex->x = -vertex->x;
    vertex->z = -vertex->z;
}

void VertexRotate270(struct Vector3* vertex) {
    float tmp = vertex->x;
    vertex->x = vertex->z;
    vertex->z = -tmp;
}

static VertexTransform gSimpleRotation[] = {
    VertexRotateNone,
    VertexRotate90,
    VertexRotate180,
    VertexRotate270,
};

static VertexTransform gSimpleRotationInv[] = {
    VertexRotateNone,
    VertexRotate270,
    VertexRotate180,
    VertexRotate90,
};

int collisionGridCollideSphere(struct Vector3* center, float radius, struct LevelCollisionGrid* grid, int collisionMask, struct CollisionResult* result) {
    float gridX = (center->x + 1.0f) * 0.5f;
    float gridY = (center->y + 2.0f) * 0.5f;
    float gridZ = (center->z - 1.0f) * -0.5f;

    float halfRadius = radius * 0.5f;

    int minX = (int)floorf(gridX - halfRadius);
    int minY = (int)floorf(gridY - halfRadius);
    int minZ = (int)floorf(gridZ - halfRadius);

    int maxX = (int)ceilf(gridX + halfRadius);
    int maxY = (int)ceilf(gridY + halfRadius);
    int maxZ = (int)ceilf(gridZ + halfRadius);
    
    if (minX < 0) {
        minX = 0;
    }

    if (minY < 0) {
        minY = 0;
    }

    if (minZ < 0) {
        minZ = 0;
    }

    int x;
    int y;
    int z;

    int didCollide = 0;

    for (x = minX; x < grid->width && x <= maxX; ++x) {
        for (z = minZ; z < grid->height && z <= maxZ; ++z) {
            struct LevelCollisionCell* cell = &grid->tiles[x * grid->height + z];
            struct LevelCollisionTile* tile = cell->tile;

            for (y = minY; tile && y < LEVEL_BLOCK_HEIGHT && y <= maxY; ++y) {
                if (tile->blocks[y]) {
                    struct Vector3 relativeCenter;
                    relativeCenter.x = center->x - x * LEVEL_GRID_SIZE;
                    relativeCenter.y = center->y - y * LEVEL_GRID_SIZE;
                    relativeCenter.z = center->z + z * LEVEL_GRID_SIZE;

                    gSimpleRotationInv[cell->rotation](&relativeCenter);

                    if (collisionColliderCollideSphere(&relativeCenter, radius, tile->blocks[y], collisionMask, result)) {
                        gSimpleRotation[cell->rotation](&relativeCenter);

                        center->x = relativeCenter.x + x * LEVEL_GRID_SIZE;
                        center->y = relativeCenter.y + y * LEVEL_GRID_SIZE;
                        center->z = relativeCenter.z - z * LEVEL_GRID_SIZE;

                        didCollide = 1;

                        if (result->contactCount == MAX_CONTACT_POINTS) {
                            return 1;
                        }
                    }
                }
            }
        }
    }

    return didCollide;
}

float collisionGridRaycast(struct LevelCollisionGrid* grid, struct Vector3* position, struct Vector3* dir, int collisionMask, float maxDistance, struct ContactPoint* hit) {
    float gridX = (position->x + 1.0f) * 0.5f;
    float gridY = (position->y + 2.0f) * 0.5f;
    float gridZ = (position->z - 1.0f) * -0.5f;
    
    int x = (int)floorf(gridX);
    int y = (int)floorf(gridY);
    int z = (int)floorf(gridZ);

    if (x < 0 || x >= grid->width ||
        y < 0 ||
        z < 0 || z >= grid->height) {
        return RAYCAST_NO_HIT;
    }

    if (y >=3 ) {
        y = 2;
    }

    while (y >= 0) {
        struct LevelCollisionCell* cell = &grid->tiles[x * grid->height + z];

        if (cell->tile && cell->tile->blocks[y]) {
            struct Vector3 relativeCenter;
            relativeCenter.x = position->x - x * LEVEL_GRID_SIZE;
            relativeCenter.y = position->y - y * LEVEL_GRID_SIZE;
            relativeCenter.z = position->z + z * LEVEL_GRID_SIZE;

            struct Vector3 rotatedDir;

            gSimpleRotationInv[cell->rotation](&relativeCenter);
            // gSimpleRotationInv[cell->rotation](&rotatedDir);


            float result = collisionColliderRaycast(cell->tile->blocks[y], &relativeCenter, dir, collisionMask, hit);

            if (result != RAYCAST_NO_HIT) {
                gSimpleRotation[cell->rotation](&hit->point);
                // gSimpleRotation[cell->rotation](&hit->normal);

                hit->point.x += x * LEVEL_GRID_SIZE;
                hit->point.y += y * LEVEL_GRID_SIZE;
                hit->point.z -= z * LEVEL_GRID_SIZE;

                return result;
            }
        }

        --y;
    }

    return RAYCAST_NO_HIT;
}