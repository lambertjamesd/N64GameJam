
#include "cadet.h"
#include "src/input/controller.h"
#include "src/input/inputfocus.h"
#include "src/graphics/renderscene.h"
#include "src/collision/collisionscene.h"
#include "geo/model.h"
#include "src/graphics/dynamic.h"
#include "src/math/mathf.h"
#include "src/audio/playersounds.h"
#include "src/audio/audio.h"

#define MAX_SHADOW_SCALE 0.6f
#define MIN_SHADOW_SCALE 0.2f

#define MAX_SHADOW_TRANS 168.0f
#define MIN_SHADOW_TRANS 94.0f

#define COYOTE_TIME 0.1f

struct DropShadowParams gCadetShadowParams = {
    MIN_SHADOW_SCALE,
    MAX_SHADOW_SCALE,
    MIN_SHADOW_TRANS,
    MAX_SHADOW_TRANS,
    CollisionLayersGeometry | CollisionLayersRobot,
};

struct Cadet gCadet;

void cadetWalk(struct Cadet* cadet);
void cadetFreefall(struct Cadet* cadet);
void cadetJump(struct Cadet* cadet);

void cadetRender(struct DynamicActor* data, struct GraphicsState* state) {
    struct Cadet* cadet = (struct Cadet*)data->data;

    Mtx* nextTransfrom = graphicsStateNextMtx(state);
    transformToMatrixL(data->transform, 1.0f / 256.0f, nextTransfrom);
    gSPMatrix(state->dl++, OS_K0_TO_PHYSICAL(nextTransfrom), G_MTX_MODELVIEW|G_MTX_MUL|G_MTX_PUSH);
    gSPDisplayList(state->dl++, Cadet_Cadet_mesh);
    gSPPopMatrix(state->dl++, G_MTX_MODELVIEW);
}

void cadetMove(struct Cadet* cadet) {
    struct Vector2 input2d = {0.0f, 0.0f};
    struct Vector2 rotatedInput;

    if (gInputMask & InputMaskCadet) {
        input2d.x = gControllerState[0].stick_x / 80.0f;
        input2d.y = -gControllerState[0].stick_y / 80.0f;
    }

    cameraGetMoveDir(&gScene.camera, &input2d, &rotatedInput);

    cadet->actor.velocity.y += GLOBAL_GRAVITY * gTimeDelta;
    struct Vector3 targetVelocity;

    targetVelocity.x = CADET_SPEED * rotatedInput.x;
    targetVelocity.y = cadet->actor.velocity.y;
    targetVelocity.z = CADET_SPEED * rotatedInput.y;

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

    int isGrounded = (cadet->actor.stateFlags & SPHERE_ACTOR_IS_GROUNDED);
    if (!isGrounded && cadet->coyoteTimer <= 0.0f) {
        cadet->state = cadetFreefall;
    } else {
        if (isGrounded) {
            cadet->coyoteTimer = COYOTE_TIME;
        } else {
            cadet->coyoteTimer -= gTimeDelta;
        }

        if ((gInputMask & InputMaskCadet) && getButtonDown(0, A_BUTTON)) {
            cadet->actor.velocity.y = CADET_JUMP_IMPULSE;
            cadet->state = cadetJump;
            cadet->actor.stateFlags |= CADET_IS_JUMPING;

            audioPlaySound(
                gPlayerSoundIds[PlayerSoundsJump],
                0.5f,
                1.0f,
                0.0f,
                10
            );
        }

        if (cadet->actor.velocity.x != 0.0f || cadet->actor.velocity.z != 0.0f) {
            cadet->rotation.x = cadet->actor.velocity.x;
            cadet->rotation.y = cadet->actor.velocity.z;
            vector2Normalize(&cadet->rotation, &cadet->rotation);
            struct Vector2 dir;
            dir.x = cadet->rotation.y;
            dir.y = cadet->rotation.x;
            quatAxisComplex(&gUp, &dir, &cadet->transform.rotation);
        }
    }
}

void cadetFreefall(struct Cadet* cadet) {
    int wasMovingUp = cadet->actor.velocity.y > 0.0f;

    cadetMove(cadet);

    if ((cadet->actor.stateFlags & CADET_IS_JUMPING) && wasMovingUp && cadet->actor.velocity.y <= 0.0f) {
        audioPlaySound(
            gPlayerSoundIds[PlayerSoundsJumpPeak],
            0.5f,
            0.75f,
            0.0f,
            10
        );
    }

    if (cadet->actor.stateFlags & SPHERE_ACTOR_IS_GROUNDED) {
        audioPlaySound(
            gPlayerSoundIds[PlayerSoundsLand],
            0.5f,
            0.8f,
            0.0f,
            10
        );

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

    dropShadowCalculate(&cadet->shadow, cadet->actor.stateFlags & SPHERE_ACTOR_IS_GROUNDED, &cadet->transform.position);

    if (gInputMask & InputMaskCadet) {
        gScene.camera.targetPosition = cadet->transform.position;

        if (gScene.camera.targetPosition.y < 0.0f) {
            gScene.camera.targetPosition.y = 0.0f;
        }
    }
}

void cadetReset(struct Vector3* startLocation) {
    timeAddListener(&gCadet.updateListener, cadetUpdate, &gCadet);
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
    dynamicActorAddToGroup(&gScene.transparentActors, &gCadet.transform, &gCadet.shadow, dropShadowRender, TransparentMaterialTypeShadow);
}

void cadetInit() {
    gCadet.shadow.params = &gCadetShadowParams;
    gCadet.actor.collisionMask = 
        CollisionLayersGeometry | 
        CollisionLayersRobot | 
        CollisionLayersMutualSwitch |
        CollisionLayersCadetSwitch |
        CollisionLayersKillPlane;
    cadetReset(&gZeroVec);
}