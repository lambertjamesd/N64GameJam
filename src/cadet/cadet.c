
#include "cadet.h"
#include "src/input/controller.h"
#include "src/input/inputfocus.h"
#include "src/graphics/renderscene.h"
#include "src/collision/collisionscene.h"

#include "src/system/memory.h"

struct Cadet gCadet;

void cadetWalk(struct Cadet* cadet);
void cadetFreefall(struct Cadet* cadet);
void cadetJump(struct Cadet* cadet);

void cadetCheckCollisions(struct Cadet* cadet) {
    struct Vector3 centerPos = cadet->transform.position;
    centerPos.y += CADET_RADIUS;

    struct CollisionResult* collisionResult = collisionSceneCollideSphere(&centerPos, CADET_RADIUS, 0);

    cadet->transform.position = centerPos;
    cadet->transform.position.y -= CADET_RADIUS;

    int nextGrounded = 0;
    cadet->anchor = 0;

    int i;
    for (i = 0; i < collisionResult->contactCount; ++i) {
        float dotDiff = vector3Dot(&cadet->velocity, &collisionResult->contacts[i].normal);

        if (dotDiff < 0.0f)
        {
            cadet->velocity.x -= dotDiff * collisionResult->contacts[i].normal.x;
            cadet->velocity.y -= dotDiff * collisionResult->contacts[i].normal.y;
            cadet->velocity.z -= dotDiff * collisionResult->contacts[i].normal.z;
        }

        if (collisionResult->contacts[i].collisionMask & CollisionLayersKillPlane) {
            cadet->transform.position = cadet->lastStableLocation;
            cadet->velocity = gZeroVec;
            nextGrounded = 0;
            cadet->anchor = 0;
            cadet->state = cadetFreefall;
        } else if (collisionResult->contacts[i].normal.y > 0.73f) {
            nextGrounded = 1;
            cadet->anchor = collisionResult->contacts[i].transform;
            cadet->lastStableLocation = cadet->transform.position;
        }
    }

    if (nextGrounded) {
        cadet->stateFlags |= CADET_IS_GROUNDED;
        if (cadet->anchor) {
            transformPointInverse(cadet->anchor, &cadet->transform.position, &cadet->relativeToAnchor);
        }
    } else {
        cadet->stateFlags &= ~CADET_IS_GROUNDED;
    }

    fastMallocReset();
}

void cadetMove(struct Cadet* cadet) {
    float inputX = 0.0f;
    float inputY = 0.0f;

    if (gInputMask & InputMaskCadet) {
        inputX = gControllerState[0].stick_x / 80.0f;
        inputY = -gControllerState[0].stick_y / 80.0f;
    }

    cadet->velocity.y += GLOBAL_GRAVITY * gTimeDelta;
    struct Vector3 targetVelocity;

    targetVelocity.x = CADET_SPEED * inputX;
    targetVelocity.y = cadet->velocity.y;
    targetVelocity.z = CADET_SPEED * inputY;

    vector3MoveTowards(
        &cadet->velocity, 
        &targetVelocity, 
        (cadet->stateFlags & CADET_IS_GROUNDED) ? 
            CADET_ACCEL * gTimeDelta :
            CADET_AIR_ACCEL * gTimeDelta,
        &cadet->velocity
    );

    if (cadet->anchor) {
        transformPoint(cadet->anchor, &cadet->relativeToAnchor, &cadet->transform.position);
    }

    vector3AddScaled(&cadet->transform.position, &cadet->velocity, gTimeDelta, &cadet->transform.position);

    cadetCheckCollisions(cadet);
}

void cadetWalk(struct Cadet* cadet) {
    cadetMove(cadet);

    if (!(cadet->stateFlags & CADET_IS_GROUNDED)) {
        cadet->state = cadetFreefall;
    } else {
        if ((gInputMask & InputMaskCadet) && getButtonDown(0, A_BUTTON)) {
            cadet->velocity.y = CADET_JUMP_IMPULSE;
            cadet->state = cadetJump;
        }
    }
}

void cadetFreefall(struct Cadet* cadet) {
    cadetMove(cadet);

    if (cadet->stateFlags & CADET_IS_GROUNDED) {
        cadet->state = cadetWalk;
    }
}

void cadetJump(struct Cadet* cadet) {
    if ((gInputMask & InputMaskCadet) && getButton(0, A_BUTTON)) {
        cadet->velocity.y += CADET_JUMP_ACCEL * gTimeDelta;
    } else {
        cadet->state = cadetFreefall;
    }

    cadetFreefall(cadet);
}

void cadetUpdate(void* cadetPtr) {
    struct Cadet* cadet = (struct Cadet*)cadetPtr;
    cadet->state(cadet);

    gScene.camera.targetPosition = cadet->transform.position;
}

void cadetReset(struct Vector3* startLocation) {
    transformIdentity(&gCadet.transform);
    
    gCadet.transform.position = *startLocation;
    gCadet.state = cadetFreefall;

    gCadet.velocity = gZeroVec;
    gCadet.lastStableLocation = *startLocation;
    gCadet.stateFlags = 0;
    gCadet.anchor = 0;
    gCadet.relativeToAnchor = gZeroVec;
}

void cadetInit() {
    timeAddListener(&gCadet.updateListener, cadetUpdate, &gCadet);
    cadetReset(&gZeroVec);
}