
#include "levelcollisiongrid.h"
#include <ultra64.h>

int collisionGridCollideSphere(struct LevelCollisionGrid* grid, struct Vector3* center, float radius, struct CollisionResult* result) {
    int minX = (int)floorf(center->x - radius);
    int minY = (int)floorf(center->y - radius);
    int minZ = (int)floorf(center->z - radius);

    int maxX = (int)ceilf(center->x + radius);
    int maxY = (int)ceilf(center->y + radius);
    int maxZ = (int)ceilf(center->z + radius);

    int x;
    int y;
    int z;

    int didCollide = 0;

    for (x = minX; x < grid->width && x <= maxX; ++x) {
        for (z = minZ; z < grid->height && z <= maxZ; ++z) {
            struct LevelCollisionTile* tile = grid->tiles[x + z * grid->width].tile;

            for (y = 0; tile && y < LEVEL_BLOCK_HEIGHT; ++y) {
                if (tile->blocks[y]) {
                    struct Vector3 relativeCenter;
                    relativeCenter.x = center->x - x * LEVEL_GRID_SIZE;
                    relativeCenter.y = center->y - y * LEVEL_GRID_SIZE;
                    relativeCenter.z = center->z - z * LEVEL_GRID_SIZE;

                    if (collisionColliderCollideSphere(tile->blocks[y], &relativeCenter, radius, result)) {
                        center->x = relativeCenter.x + x * LEVEL_GRID_SIZE;
                        center->y = relativeCenter.y + y * LEVEL_GRID_SIZE;
                        center->z = relativeCenter.z + z * LEVEL_GRID_SIZE;

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