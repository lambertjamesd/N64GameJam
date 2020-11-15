
#include "sphereactor.h"
#include "src/system/memory.h"

enum SphereActorCollideResult sphereActorCollideScene(struct SphereActor* actor, struct Vector3* position) {
    struct Vector3 centerPos = *position;
    centerPos.y += actor->radius;

    enum SphereActorCollideResult result = SphereActorCollideResultNone;

    struct CollisionResult* collisionResult = collisionSceneCollideSphere(&centerPos, actor->radius, actor->collisionMask);

    *position = centerPos;
    position->y -= actor->radius;

    int nextGrounded = 0;
    actor->anchor = 0;

    int i;
    for (i = 0; i < collisionResult->contactCount; ++i) {
        float dotDiff = vector3Dot(&actor->velocity, &collisionResult->contacts[i].normal);

        if (dotDiff < 0.0f)
        {
            actor->velocity.x -= dotDiff * collisionResult->contacts[i].normal.x;
            actor->velocity.y -= dotDiff * collisionResult->contacts[i].normal.y;
            actor->velocity.z -= dotDiff * collisionResult->contacts[i].normal.z;
        }

        if (collisionResult->contacts[i].collisionMask & CollisionLayersKillPlane) {
            result = SphereActorCollideResultKill;
            nextGrounded = 0;
        } else if (collisionResult->contacts[i].normal.y > 0.95f) {
            nextGrounded = 1;
            actor->anchor = collisionResult->contacts[i].transform;
            actor->lastStableLocation = *position;
        }
    }

    if (nextGrounded) {
        actor->stateFlags |= SPHERE_ACTOR_IS_GROUNDED;
        if (actor->anchor) {
            transformPointInverse(actor->anchor, position, &actor->relativeToAnchor);
        }
    } else {
        actor->stateFlags &= ~SPHERE_ACTOR_IS_GROUNDED;
    }

    fastMallocReset();

    return result;
}