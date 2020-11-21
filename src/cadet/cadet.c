
#include "cadet.h"
#include "geo/model.h"
#include "src/audio/audio.h"
#include "src/audio/playersounds.h"
#include "src/collision/collisionscene.h"
#include "src/graphics/dynamic.h"
#include "src/graphics/renderscene.h"
#include "src/input/controller.h"
#include "src/input/inputfocus.h"
#include "src/math/mathf.h"
#include "src/puzzle/entranceexit.h"

#define MAX_SHADOW_SCALE 0.6f
#define MIN_SHADOW_SCALE 0.2f

#define MAX_SHADOW_TRANS 168.0f
#define MIN_SHADOW_TRANS 94.0f

#define WALK_CYCLE_DUR      0.25f
#define WALK_JUMP_HEIGHT    0.15f
#define WALK_SWAY_ANGLE     (M_PI * 0.05f)

#define COYOTE_TIME 0.1f

#define CADET_TURN_RATE     (M_PI * 3.0f)

struct DropShadowParams gCadetShadowParams = {
    MIN_SHADOW_SCALE,
    MAX_SHADOW_SCALE,
    MIN_SHADOW_TRANS,
    MAX_SHADOW_TRANS,
    CollisionLayersGeometry | CollisionLayersRobot,
};

struct Cadet gCadet;

struct Vector2 gCadetMaxRotation;

void cadetWalk(struct Cadet* cadet);
void cadetFreefall(struct Cadet* cadet);
void cadetJump(struct Cadet* cadet);
void cadetRespawn(struct Cadet* cadet);

void cadetRender(struct DynamicActor* data, struct GraphicsState* state) {
    struct Cadet* cadet = (struct Cadet*)data->data;

    if (cadet->actor.stateFlags & CADET_IS_INVISIBLE) {
        return;
    }

    Mtx* reflectMatrix = graphicsStateNextMtx(state);
    LookAt* nextLookat = graphicsStateNextLookat(state);

    guLookAtReflect(reflectMatrix, nextLookat, 
        0,
        gScene.camera.transform.position.y,
        0,
        0,
        cadet->transform.position.y,
        2,
        0,
        1,
        0
    );

    Mtx* nextTransfrom = graphicsStateNextMtx(state);

    if (cadet->teleport.flags & TELEPORT_FLAG_ACTIVE) {
        teleportEffectCreateTransform(&cadet->teleport, &cadet->transform, nextTransfrom);
    } else {
        struct BasicTransform walkTransform;
        struct BasicTransform combined;
        walkAnimRelativeTransform(&cadet->walkAnim, &walkTransform);
        transformConcat(data->transform, &walkTransform, &combined);
        transformToMatrixL(&combined, 1.0f / 256.0f, nextTransfrom);
    }

    gSPLookAt(state->dl++, K0_TO_PHYS(nextLookat));
    gSPMatrix(state->dl++, OS_K0_TO_PHYSICAL(nextTransfrom), G_MTX_MODELVIEW|G_MTX_MUL|G_MTX_PUSH);
    gSPDisplayList(state->dl++, Cadet_Cadet_mesh);
    gSPPopMatrix(state->dl++, G_MTX_MODELVIEW);
}

void cadetUpdateRotation(struct Cadet* cadet) {
    if (cadet->actor.velocity.x != 0.0f || cadet->actor.velocity.z != 0.0f) {
        struct Vector2 targetRotation;
        targetRotation.x = cadet->actor.velocity.x;
        targetRotation.y = cadet->actor.velocity.z;
        vector2Normalize(&targetRotation, &targetRotation);
        vector2RotateTowards(&cadet->rotation, &targetRotation, &gCadetMaxRotation, &cadet->rotation);

        struct Vector2 dir;
        dir.x = cadet->rotation.y;
        dir.y = cadet->rotation.x;
        quatAxisComplex(&gUp, &dir, &cadet->transform.rotation);
    }
}

void cadetUpdatefootstepSound(struct Cadet* cadet) {
    int isGrounded = (cadet->actor.stateFlags & SPHERE_ACTOR_IS_GROUNDED);
    int isOnGround = isGrounded && cadet->coyoteTimer > 0.0f;

    int nextSound = -1;
    float soundVolume = 0.0f;

    if (isOnGround) {
        struct Vector2 horzVel;
        horzVel.x = cadet->actor.velocity.x;
        horzVel.y = cadet->actor.velocity.z;

        float moveSpeed = vector2Dot(&horzVel, &horzVel);
        int playingState = audioPlayState(gPlayerSoundIds[PlayerSoundsFootsteps]);

        if (moveSpeed != 0.0f) {
            if (cadet->actor.groundCollisionMask & CollisionLayersRobot) {
                nextSound = PlayerSoundsFootstepsMetal;
            } else {
                nextSound = PlayerSoundsFootsteps;
            }
            soundVolume = sqrtf(moveSpeed) / CADET_SPEED;
        }
    }

    if (nextSound != cadet->footstepSound) {
        if (cadet->footstepSound != -1) {
            audioStopSound(gPlayerSoundIds[cadet->footstepSound]);
        }

        cadet->footstepSound = nextSound;
    }

    if (cadet->footstepSound != -1) {
        audioPlaySound(
            cadet->footstepSound,
            0.5f,
            soundVolume,
            0.0f,
            10
        );
    }
}

void cadetMove(struct Cadet* cadet) {
    struct Vector2 input2d = {0.0f, 0.0f};
    struct Vector2 rotatedInput;

    if (gInputMask & InputMaskCadet) {
        input2d.x = gControllerState[0].stick_x / 80.0f;
        input2d.y = -gControllerState[0].stick_y / 80.0f;
    }

    if (cadet->actor.anchor) {
        transformPoint(cadet->actor.anchor, &cadet->actor.relativeToAnchor, &cadet->transform.position);
    }

    cameraGetMoveDir(&gScene.camera, &input2d, &rotatedInput);

    cadet->accumTime += gTimeDelta;

    while (cadet->accumTime >= MIN_DELTA_TIME) {
        cadet->actor.velocity.y += cadet->gravity * MIN_DELTA_TIME;
        struct Vector3 targetVelocity;

        float speed = (cadet->actor.stateFlags & SPHERE_ACTOR_IS_GROUNDED) ? CADET_SPEED : CADET_AIR_SPEED;

        targetVelocity.x = speed * rotatedInput.x;
        targetVelocity.y = cadet->actor.velocity.y;
        targetVelocity.z = speed * rotatedInput.y;

        vector3MoveTowards(
            &cadet->actor.velocity, 
            &targetVelocity, 
            (cadet->actor.stateFlags & SPHERE_ACTOR_IS_GROUNDED) ? 
                CADET_ACCEL * MIN_DELTA_TIME :
                CADET_AIR_ACCEL * MIN_DELTA_TIME,
            &cadet->actor.velocity
        );

        vector3AddScaled(&cadet->transform.position, &cadet->actor.velocity, MIN_DELTA_TIME, &cadet->transform.position);
        cadet->accumTime -= MIN_DELTA_TIME;
    }

    enum SphereActorCollideResult colliderResult = sphereActorCollideScene(&cadet->actor, &cadet->transform.position);

    if (colliderResult == SphereActorCollideResultKill) {
        audioPlaySound(
            gPlayerSoundIds[PlayerCadetFall],
            0.5f,
            1.0f,
            0.0f,
            10
        );

        teleportEffectStart(&cadet->teleport, TELEPORT_FLAG_QUICK);
        cadet->state = cadetRespawn;
        cadet->actor.velocity = gZeroVec;
    }
}

void cadetWalk(struct Cadet* cadet) {
    cadetMove(cadet);

    int isGrounded = (cadet->actor.stateFlags & SPHERE_ACTOR_IS_GROUNDED);
    if (!isGrounded && cadet->coyoteTimer <= 0.0f) {
        cadet->state = cadetFreefall;
        walkAnimUpdate(&cadet->walkAnim, 0.0f);
    } else {
        if (isGrounded) {
            cadet->coyoteTimer = COYOTE_TIME;
        } else {
            cadet->coyoteTimer -= gTimeDelta;
        }

        if ((gInputMask & InputMaskCadet) && getButtonDown(0, A_BUTTON)) {
            cadet->actor.velocity.y = CADET_JUMP_IMPULSE;
            cadet->state = cadetJump;
            walkAnimUpdate(&cadet->walkAnim, 0.0f);
            cadet->actor.stateFlags |= CADET_IS_JUMPING;

            audioPlaySound(
                gPlayerSoundIds[PlayerSoundsJump],
                0.5f,
                1.0f,
                0.0f,
                10
            );
        } else {
            walkAnimUpdate(&cadet->walkAnim, sqrtf(
                cadet->actor.velocity.x * cadet->actor.velocity.x +
                cadet->actor.velocity.z * cadet->actor.velocity.z
            ));
        }

        cadetUpdateRotation(cadet);
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

    cadetUpdateRotation(cadet);

    if (cadet->actor.stateFlags & SPHERE_ACTOR_IS_GROUNDED) {
        audioPlaySound(
            gPlayerSoundIds[(cadet->actor.groundCollisionMask & CollisionLayersRobot) ? 
                PlayerSoundsLandMetal :
                PlayerSoundsLand
            ],
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
        cadet->gravity = CADET_JUMP_ACCEL + GLOBAL_GRAVITY;
    } else {
        cadet->gravity = GLOBAL_GRAVITY;
        cadet->state = cadetFreefall;
    }

    cadetFreefall(cadet);
}

void cadetIdle(struct Cadet* cadet) {

}

void cadetTeleportIn(struct Cadet* cadet) {
    if (cadet->actor.lastStableAnchor) {
        transformPoint(cadet->actor.lastStableAnchor, &cadet->actor.lastStableLocation, &cadet->transform.position);
    }

    if (!teleportEffectUpdate(&gCadet.teleport)) {
        cadet->state = cadetFreefall;
        cadet->actor.stateFlags &= ~CADET_IS_CUTSCENE;
    }
}

void cadetTeleportOut(struct Cadet* cadet) {
    struct Vector3 targetCenter;

    targetCenter.x = gCadetExit.transform.position.x;
    targetCenter.y = cadet->transform.position.y;
    targetCenter.z = gCadetExit.transform.position.z;

    vector3MoveTowards(&cadet->transform.position, &targetCenter, gTimeDelta * 0.5f, &cadet->transform.position);

    if (!teleportEffectUpdate(&gCadet.teleport)) {
        cadet->state = cadetIdle;
        cadet->actor.stateFlags &= ~CADET_IS_CUTSCENE;
        cadet->actor.stateFlags |= CADET_IS_INVISIBLE;
    }
}

void cadetRespawn(struct Cadet* cadet) {
    if (!teleportEffectUpdate(&cadet->teleport)) {
        cadet->state = cadetTeleportIn;
        teleportEffectStart(&cadet->teleport, TELEPORT_FLAG_REVERSE | TELEPORT_FLAG_QUICK);

        if (cadet->actor.lastStableAnchor) {
            transformPoint(cadet->actor.lastStableAnchor, &cadet->actor.lastStableLocation, &cadet->transform.position);
        } else {
            cadet->transform.position = cadet->actor.lastStableLocation;
        }

        cadet->actor.velocity = gZeroVec;
        cadet->actor.anchor = 0;
    }
}

void cadetUpdate(void* cadetPtr) {
    struct Cadet* cadet = (struct Cadet*)cadetPtr;
    cadet->state(cadet);

    cadetUpdatefootstepSound(cadet);

    dropShadowCalculate(&cadet->shadow, cadet->actor.stateFlags & SPHERE_ACTOR_IS_GROUNDED, &cadet->transform.position);
}

void cadetReset(struct Vector3* startLocation) {
    timeAddListener(&gCadet.updateListener, cadetUpdate, &gCadet, TimeUpdateGroupWorld);
    transformIdentity(&gCadet.transform);
    
    gCadet.transform.position = *startLocation;
    gCadet.state = cadetTeleportIn;

    gCadet.rotation.x = 1.0f;
    gCadet.rotation.y = 0.0f;

    gCadet.transform.position.y += 0.5f;

    teleportEffectStart(&gCadet.teleport, TELEPORT_FLAG_REVERSE);
    walkAnimInit(&gCadet.walkAnim, 
        WALK_CYCLE_DUR, 
        CADET_SPEED, 
        WALK_JUMP_HEIGHT, 
        WALK_SWAY_ANGLE
    );

    gCadet.actor.radius = CADET_RADIUS;
    gCadet.actor.velocity = gZeroVec;
    gCadet.actor.lastStableLocation = *startLocation;
    gCadet.actor.stateFlags = CADET_IS_CUTSCENE;
    gCadet.actor.anchor = 0;
    gCadet.actor.lastStableAnchor = 0;
    gCadet.actor.relativeToAnchor = gZeroVec;

    gCadet.gravity = GLOBAL_GRAVITY;
    gCadet.accumTime = 0.0f;
    gCadet.footstepSound = -1;

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

    gCadetMaxRotation.x = cosf(MIN_DELTA_TIME * CADET_TURN_RATE);
    gCadetMaxRotation.y = sinf(MIN_DELTA_TIME * CADET_TURN_RATE);
    cadetReset(&gZeroVec);
}

void cadetFinishLevel(struct Cadet* cadet) {
    teleportEffectStart(&gCadet.teleport, 0);
    cadet->state = cadetTeleportOut;
}