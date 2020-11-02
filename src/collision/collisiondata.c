
#include "collisiondata.h"

#include "meshcollision.h"

int collisionColliderCollideSphere(struct CollisionCollider* collider, struct Vector3* center, float radius, struct CollisionResult* result) {
    switch (collider->type) {
        case ColliderTypeMesh:
            return collisionMeshCollideSphere(&collider->mesh, center, radius, result);
        case ColliderTypeBox:
            // TODO
            return 0;
    }

    return 0;
}