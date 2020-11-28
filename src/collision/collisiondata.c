
#include "collisiondata.h"

#include "meshcollision.h"
#include "boxcollision.h"


void collisionColliderCalculateBox(struct CollisionCollider* collider, struct Vector3* center, struct Box* out) {
    boxOffset(&collider->box, center, out);
}

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
    if (!((collider->collider->collisionMask | collider->triggerMask) & collisionMask)) {
        return 0;
    }

    if (center->x + radius < collider->lastBoundingBox.min.x ||
        center->y + radius < collider->lastBoundingBox.min.y ||
        center->z + radius < collider->lastBoundingBox.min.z ||
        center->x - radius > collider->lastBoundingBox.max.x ||
        center->y - radius > collider->lastBoundingBox.max.y ||
        center->z - radius > collider->lastBoundingBox.max.z) {
        return 0;
    }

    struct Vector3 transformedCenter;
    transformPointInverse(collider->transform, center, &transformedCenter);

    if ((collider->triggerMask & collisionMask) && collider->trigger) {
        if (collisionColliderOverlapSphere(collider->collider, &transformedCenter, radius)) {
            collider->trigger(collider->data, center);
        }
    }


    int startCount = result->contactCount;

    struct Vector3 transformedCenterCopy = transformedCenter;

    if (collisionColliderCollideSphere(&transformedCenter, radius, collider->collider, collisionMask, result)) {
        transformPoint(collider->transform, &transformedCenter, center);


        int i;
        for (i = startCount; i < result->contactCount; ++i) {
            result->contacts[i].transform = collider->transform;
            transformDirection(collider->transform, &result->contacts[i].normal, &result->contacts[i].normal);
        }

        return 1;
    } else {
        return 0;
    }
}

float collisionColliderRaycast(struct CollisionCollider* collider, struct Vector3* origin, struct Vector3* dir, int collisionMask, struct ContactPoint* contact) {
    if (!(collider->collisionMask & collisionMask)) {
        return RAYCAST_NO_HIT;
    }

    float result = RAYCAST_NO_HIT;

    switch (collider->type) {
        case ColliderTypeMesh:
            result = collisionMeshRaycast(&collider->mesh, origin, dir, contact);
            break;
        case ColliderTypeBox:
            result = collisionBoxRaycast(&collider->box, origin, dir, contact);
            break;
    }

    if (result != RAYCAST_NO_HIT) {
        contact->collisionMask = collider->collisionMask;
        contact->transform = 0;
        return result;
    }

    return RAYCAST_NO_HIT;
}

float collisionTransColliderRaycast(struct CollisionTransformedCollider* collider, struct Vector3* origin, struct Vector3* dir, int collisionMask, struct ContactPoint* contact) {
    if (!(collider->collider->collisionMask & collisionMask)) {
        return RAYCAST_NO_HIT;
    }

    struct Vector3 relativeOrigin;
    struct Vector3 relativeDir;

    transformPointInverse(collider->transform, origin, &relativeOrigin);
    transformDirectionInverse(collider->transform, dir, &relativeDir);

    float result = collisionColliderRaycast(collider->collider, &relativeOrigin, &relativeDir, collisionMask, contact);

    if (result != RAYCAST_NO_HIT) {
        transformPoint(collider->transform, &contact->point, &contact->point);
        transformDirection(collider->transform, &contact->normal, &contact->normal);
        contact->transform = collider->transform;
    }

    return result;
}