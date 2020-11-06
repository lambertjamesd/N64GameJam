
#include "collisiondata.h"

#include "meshcollision.h"
#include "boxcollision.h"

int collisionColliderCollideSphere(struct Vector3* center, float radius, struct CollisionCollider* collider, int collisionMask, struct CollisionResult* result) {
    if (!(collider->collisionMask & collisionMask)) {
        return 0;
    }

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

int collisionColliderOverlapSphere(struct CollisionCollider* collider, struct Vector3* center, float radius) {
    if (collider->type == ColliderTypeBox) {
        return collisionBoxOverlapSphere(&collider->box, center, radius);
    }

    return 0;
}

int collisionTransColliderCollideSphere(struct Vector3* center, float radius, struct CollisionTransformedCollider* collider, int collisionMask, struct CollisionResult* result) {
    if (!(collider->collider->collisionMask & collisionMask)) {
        return 0;
    }

    struct Vector3 transformedCenter;
    transformPointInverse(collider->transform, center, &transformedCenter);

    if (collider->trigger) {
        if (collisionColliderOverlapSphere(collider->collider, &transformedCenter, radius)) {
            collider->trigger(collider->data);
        }

        return 0;
    }


    int startCount = result->contactCount;

    struct Vector3 transformedCenterCopy = transformedCenter;

    if (collisionColliderCollideSphere(&transformedCenter, radius, collider->collider, collisionMask, result)) {
        transformPoint(collider->transform, &transformedCenter, center);


        int i;
        for (i = startCount; i < result->contactCount; ++i) {
            result->contacts[i].transform = collider->transform;
        }

        return 1;
    } else {
        return 0;
    }
}