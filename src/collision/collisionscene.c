
#include "collisionscene.h"
#include "src/system/memory.h"

#include "geo/geo.h"
#include "meshcollision.h"

struct CollisionResult* collisionSceneCollideSphere(struct Vector3* position, float radius, int collisionMask) {
    struct CollisionResult* result = FAST_MALLOC_STRUCT(struct CollisionResult);
    result->contacts = FAST_MALLOC_STRUCT_ARRAY(struct ContactPoint, MAX_CONTACT_POINTS);
    result->contactCount = 0;

    if (position->y - radius < -2.0f) {
        result->contactCount = 1;

        result->contacts[0].normal = gUp;
        result->contacts[0].point = *position;
        result->contacts[0].point.y = -2.0f;
        result->contacts[0].transform = 0;

        position->y = radius - 2.0f;
    }

    collisionColliderCollideSphere(&_solid_block_collider, position, radius, result);

    return result;
}