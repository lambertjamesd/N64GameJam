
#include "cadet.h"
#include "src/input/controller.h"
#include "src/input/inputfocus.h"
#include "src/graphics/renderscene.h"
#include "src/collision/collisionscene.h"
#include "geo/model.h"
#include "src/graphics/dynamic.h"

struct Cadet gCadet;

void cadetWalk(struct Cadet* cadet);
void cadetFreefall(struct Cadet* cadet);
void cadetJump(struct Cadet* cadet);

void cadetRender(struct DynamicActor* data, struct GraphicsState* state) {
    Mtx* nextTransfrom = graphicsStateNextMtx(state);

    transformToMatrixL(data->transform, 1.0f / 256.0f, nextTransfrom);
    gSPMatrix(state->dl++, OS_K0_TO_PHYSICAL(nextTransfrom), G_MTX_MODELVIEW|G_MTX_MUL|G_MTX_PUSH);
    gSPDisplayList(state->dl++, Cadet_Cadet_mesh);
    gSPPopMatrix(state->dl++, G_MTX_MODELVIEW);
}

void cadetMove(struct Cadet* cadet) {
    float inputX = 0.0f;
    float inputY = 0.0f;

    if (gInputMask & InputMaskCadet) {
        inputX = gControllerState[0].stick_x / 80.0f;
        inputY = -gControllerState[0].stick_y / 80.0f;
    }

    cadet->actor.velocity.y += GLOBAL_GRAVITY * gTimeDelta;
    struct Vector3 targetVelocity;

    targetVelocity.x = CADET_SPEED * inputX;
    targetVelocity.y = cadet->actor.velocity.y;
    targetVelocity.z = CADET_SPEED * inputY;

    vector3MoveTowards(
        &cadet->actor.velocity, 
        &targetVelocity, 
        (cadet->actor.stateFlags & SPHERE_ACTOR_IS_GROUNDED) ? 
            CADET_ACCEL * gTimeDelta :
            CADET_AIR_ACCEL * gTimeDelta,
        &cadet->actor.velocity
    );

    if (cadet->actor.anchor) {
        transformPoint(cadet->actor.anchor, &cadet->actor.relativeToAnchor, &cadet->transform.position);
    }

    vector3AddScaled(&cadet->transform.position, &cadet->actor.velocity, gTimeDelta, &cadet->transform.position);

    sphereActorCollideScene(&cadet->actor, &cadet->transform.position);
}

void cadetWalk(struct Cadet* cadet) {
    cadetMove(cadet);

    if (!(cadet->actor.stateFlags & SPHERE_ACTOR_IS_GROUNDED)) {
        cadet->state = cadetFreefall;
    } else {
        if ((gInputMask & InputMaskCadet) && getButtonDown(0, A_BUTTON)) {
            cadet->actor.velocity.y = CADET_JUMP_IMPULSE;
            cadet->state = cadetJump;
        }
    }
}

void cadetFreefall(struct Cadet* cadet) {
    cadetMove(cadet);

    if (cadet->actor.stateFlags & SPHERE_ACTOR_IS_GROUNDED) {
        cadet->state = cadetWalk;
    }
}

void cadetJump(struct Cadet* cadet) {
    if ((gInputMask & InputMaskCadet) && getButton(0, A_BUTTON)) {
        cadet->actor.velocity.y += CADET_JUMP_ACCEL * gTimeDelta;
    } else {
        cadet->state = cadetFreefall;
    }

    cadetFreefall(cadet);
}

void cadetUpdate(void* cadetPtr) {
    struct Cadet* cadet = (struct Cadet*)cadetPtr;
    cadet->state(cadet);

    if (gInputMask & InputMaskCadet) {
        if (getButtonDown(0, L_TRIG | Z_TRIG)) {
            gInputMask = INPUT_MASK_ROBOT;
        }

        gScene.camera.targetPosition = cadet->transform.position;
    }
}

void cadetReset(struct Vector3* startLocation) {
    transformIdentity(&gCadet.transform);
    
    gCadet.transform.position = *startLocation;
    gCadet.state = cadetFreefall;

    gCadet.actor.radius = CADET_RADIUS;
    gCadet.actor.velocity = gZeroVec;
    gCadet.actor.lastStableLocation = *startLocation;
    gCadet.actor.stateFlags = 0;
    gCadet.actor.anchor = 0;
    gCadet.actor.relativeToAnchor = gZeroVec;

    dynamicActorAddToGroup(&gScene.dynamicActors, &gCadet.transform, &gCadet, cadetRender, MATERIAL_INDEX_NOT_BATCHED);
}

void cadetInit() {
    timeAddListener(&gCadet.updateListener, cadetUpdate, &gCadet);
    cadetReset(&gZeroVec);
}