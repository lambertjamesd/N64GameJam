
#include "collisiondata.h"

#include "meshcollision.h"
#include "boxcollision.h"

int collisionColliderCollideSphere(struct CollisionCollider* collider, struct Vector3* center, float radius, struct CollisionResult* result) {
    int didCollide = 0;
    int contactsBefore = result->contactCount;
    switch (collider->type) {
        case ColliderTypeMesh:
            didCollide = collisionMeshCollideSphere(&collider->mesh, center, radius, result);
            break;
        case ColliderTypeBox:
            didCollide = collisionBoxCollideSphere(&collider->box, center, radius, result);
            break;
    }

    int i;

    for (i = contactsBefore; i < result->contactCount; ++i) {
        result->contacts[i].collisionMask = collider->collisionMask;
    }

    return didCollide;
}