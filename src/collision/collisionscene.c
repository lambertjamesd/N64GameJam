
#include "collisionscene.h"
#include "src/system/memory.h"

#include "meshcollision.h"

static struct LevelCollisionGrid* gLevelCollisionGrid;
struct SparseCollisionGrid gSparseCollisionGrid;

struct CollisionResult* collisionSceneCollideSphere(struct Vector3* position, float radius, int collisionMask) {
    struct CollisionResult* result = FAST_MALLOC_STRUCT(struct CollisionResult);
    result->contacts = FAST_MALLOC_STRUCT_ARRAY(struct ContactPoint, MAX_CONTACT_POINTS);
    result->contactCount = 0;

    if (position->y - radius < KILL_PLANE_HEIGHT) {
        result->contactCount = 1;

        result->contacts[0].normal = gUp;
        result->contacts[0].point = *position;
        result->contacts[0].point.y = KILL_PLANE_HEIGHT;
        result->contacts[0].transform = 0;
        result->contacts[0].collisionMask = CollisionLayersKillPlane;

        position->y = radius + KILL_PLANE_HEIGHT;
    }

    if (gLevelCollisionGrid) {
        collisionGridCollideSphere(position, radius, gLevelCollisionGrid, collisionMask, result);
    }

    if (result->contactCount != MAX_CONTACT_POINTS) {
        collisionSparseGridCollideSphere(position, radius, &gSparseCollisionGrid, collisionMask, result);
    }

    return result;
}

float collisionSceneRaycast(struct Vector3* position, struct Vector3* dir, int collisionMask, float maxDistance, struct ContactPoint* hit) {
    struct ContactPoint sparseContact;

    if (gLevelCollisionGrid) {
        float gridHit = collisionGridRaycast(gLevelCollisionGrid, position, dir, collisionMask, maxDistance, hit);
        float sparseHit = collisionSparseGridRaycast(&gSparseCollisionGrid, position, dir, collisionMask, maxDistance, &sparseContact);

        if (gridHit < sparseHit) {
            return gridHit;
        } else {
            *hit = sparseContact;
            return sparseHit;
        }

    } else {
        return collisionSparseGridRaycast(&gSparseCollisionGrid, position, dir, collisionMask, maxDistance, hit);
    }
}

void collisionSceneUseGrid(struct LevelCollisionGrid* grid) {
    gLevelCollisionGrid = grid;
    sparseCollisionInit(&gSparseCollisionGrid);
}