
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

#define SPLASH_CHECK_THRESH -0.1f

struct DropShadowParams gCadetShadowParams = {
    MIN_SHADOW_SCALE,
    MAX_SHADOW_SCALE,
    MIN_SHADOW_TRANS,
    MAX_SHADOW_TRANS,
    CollisionLayersGeometry | CollisionLayersRobot,
};

struct RocketTrailParameters gCadetTrailParameters = {
    TransparentMaterialTypeSmoke,
    {0.0f, 0.2f, -0.1f}, 
    0.5f,
    0.6f,
    0.15f,
    0.5f,
    0.02f,
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
        state->cameraPos->y,
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

        struct Vector3 rotation;

        if (cadet->actor.anchor) {
            rotation.x = cadet->rotation.x;
            rotation.y = 0.0f;
            rotation.z = cadet->rotation.y;
            quatMultVector(&cadet->actor.anchor->rotation, &rotation, &rotation);
            cadet->rotation.x = rotation.x;
            cadet->rotation.y = rotation.z;
        }

        vector2Normalize(&targetRotation, &targetRotation);
        vector2RotateTowards(&cadet->rotation, &targetRotation, &gCadetMaxRotation, &cadet->rotation);

        struct Vector2 dir;
        dir.x = cadet->rotation.y;
        dir.y = cadet->rotation.x;
        quatAxisComplex(&gUp, &dir, &cadet->transform.rotation);

        if (cadet->actor.anchor) {
            struct Quaternion quatInv;
            
            rotation.x = cadet->rotation.x;
            rotation.y = 0.0f;
            rotation.z = cadet->rotation.y;
            quatConjugate(&cadet->actor.anchor->rotation, &quatInv);
            quatMultVector(&quatInv, &rotation, &rotation);
            cadet->rotation.x = rotation.x;
            cadet->rotation.y = rotation.z;
        }
    } else if (cadet->actor.anchor) {
        struct Vector3 rotation;

        rotation.x = cadet->rotation.x;
        rotation.y = 0.0f;
        rotation.z = cadet->rotation.y;

        if (cadet->actor.anchor) {
            quatMultVector(&cadet->actor.anchor->rotation, &rotation, &rotation);
        }

        struct Vector2 dir;
        dir.x = rotation.z;
        dir.y = rotation.x;
        quatAxisComplex(&gUp, &dir, &cadet->transform.rotation);
    }
}

void cadetUpdatefootstepSound(struct Cadet* cadet) {
    int isGrounded = (cadet->actor.stateFlags & SPHERE_ACTOR_IS_GROUNDED);
    int isOnGround = isGrounded || cadet->coyoteTimer > 0.0f;

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
            soundVolume = 0.4f * sqrtf(moveSpeed) / CADET_SPEED;
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

void cadetTargetVelocity(struct Cadet* cadet, struct Vector3* output) {
    struct Vector2 rotatedInput;

    if (cadet->controllerIndex != -1 && gInputMask[cadet->controllerIndex] & InputMaskPlayer) {
        struct Vector2 input2d = getJoystick(cadet->controllerIndex);
        cameraGetMoveDir(&gScene.camera[cadet->controllerIndex], &input2d, &rotatedInput);
    } else {
        rotatedInput.x = 0.0f;
        rotatedInput.y = 0.0f;
    }

    float speed = (cadet->actor.stateFlags & SPHERE_ACTOR_IS_GROUNDED) ? CADET_SPEED : CADET_AIR_SPEED;

    output->x = speed * rotatedInput.x;
    output->y = cadet->actor.velocity.y;
    output->z = speed * rotatedInput.y;
}

void cadetMove(struct Cadet* cadet) {
    struct Vector3 rotation;

    rotation.x = cadet->rotation.x;
    rotation.y = 0.0f;
    rotation.z = cadet->rotation.y;

    if (cadet->actor.anchor) {
        transformPoint(cadet->actor.anchor, &cadet->actor.relativeToAnchor, &cadet->transform.position);
        quatMultVector(&cadet->actor.anchor->rotation, &rotation, &rotation);
    }

    cadet->accumTime += gTimeDelta;

    struct Vector3 targetVelocity;
    cadetTargetVelocity(cadet, &targetVelocity);

    while (cadet->accumTime >= MIN_DELTA_TIME) {
        cadet->actor.velocity.y += cadet->gravity * MIN_DELTA_TIME;
        targetVelocity.y = cadet->actor.velocity.y;

        float accel;
        int isBackwards = vector3Dot(&cadet->actor.velocity, &targetVelocity);

        if (cadet->actor.stateFlags & SPHERE_ACTOR_IS_GROUNDED) {
            if (isBackwards <= 0.0f) {
                accel = CADET_BACKACCEL * MIN_DELTA_TIME;
            } else {
                accel = CADET_ACCEL * MIN_DELTA_TIME;
            }
        } else {
            if (isBackwards) {
                accel = CADET_AIR_BACKACCEL * MIN_DELTA_TIME;
            } else {
                accel = CADET_AIR_ACCEL * MIN_DELTA_TIME;
            }
        }

        vector3MoveTowards(
            &cadet->actor.velocity, 
            &targetVelocity, 
            accel,
            &cadet->actor.velocity
        );

        vector3AddScaled(&cadet->transform.position, &cadet->actor.velocity, MIN_DELTA_TIME, &cadet->transform.position);
        cadet->accumTime -= MIN_DELTA_TIME;
    }

    enum SphereActorCollideResult colliderResult = sphereActorCollideScene(&cadet->actor, &cadet->transform.position);

    if (cadet->actor.anchor) {
        struct Quaternion quatInv;
        quatConjugate(&cadet->actor.anchor->rotation, &quatInv);
        quatMultVector(&quatInv, &rotation, &rotation);
    }

    cadet->rotation.x = rotation.x;
    cadet->rotation.y = rotation.z;

    vector2Normalize(&cadet->rotation, &cadet->rotation);

    if (colliderResult == SphereActorCollideResultKill) {
        audioPlaySound(
            gPlayerSoundIds[PlayerCadetFall],
            0.5f,
            0.7f,
            0.0f,
            10
        );

        teleportEffectStart(&cadet->teleport, TELEPORT_FLAG_QUICK);
        respawnEffectStart(&cadet->respawn, &cadet->transform, 1.0f, &cadet->teleport);
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

        if (getButtonDown(cadet->controllerIndex, A_BUTTON) && (gInputMask[cadet->controllerIndex] & InputMaskPlayer)) {
            struct Vector3 targetVelocity;
            cadetTargetVelocity(cadet, &targetVelocity);

            vector3MoveTowards(&cadet->actor.velocity, &targetVelocity, CADET_HORZ_IMPULSE, &cadet->actor.velocity);

            rocketTrailStart(&cadet->jumpTrail, &cadet->transform, &gCadetTrailParameters, CADET_PARTICLE_TRAIL_COUNT, 0);
            cadet->jumpTrail.alpha = 160;
            cadet->actor.velocity.y = CADET_JUMP_IMPULSE;
            cadet->state = cadetJump;
            cadet->coyoteTimer = 0.0f;
            walkAnimUpdate(&cadet->walkAnim, 0.0f);
            cadet->actor.stateFlags |= CADET_IS_JUMPING;

            audioPlaySound(
                gPlayerSoundIds[PlayerSoundsJump],
                0.5f,
                0.4f,
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
            0.2f,
            0.0f,
            10
        );

        rocketTrailStop(&cadet->jumpTrail);
    }

    cadetUpdateRotation(cadet);

    if (cadet->actor.stateFlags & SPHERE_ACTOR_IS_GROUNDED) {
        int isOnMetal = (cadet->actor.groundCollisionMask & CollisionLayersRobot);
        audioPlaySound(
            gPlayerSoundIds[isOnMetal ? 
                PlayerSoundsLandMetal :
                PlayerSoundsLand
            ],
            0.5f,
            isOnMetal ? 0.6f : 0.5f,
            0.0f,
            10
        );

        cadet->state = cadetWalk;
        rocketTrailStop(&cadet->jumpTrail);
    }
}

void cadetJump(struct Cadet* cadet) {
    if (getButton(cadet->controllerIndex, A_BUTTON) && (gInputMask[cadet->controllerIndex] & InputMaskPlayer)) {
        cadet->gravity = CADET_JUMP_ACCEL + GLOBAL_GRAVITY;
    } else {
        cadet->gravity = GLOBAL_GRAVITY;
        cadet->state = cadetFreefall;
    }

    cadetFreefall(cadet);
}

void cadetIdle(struct Cadet* cadet) {

}


void cadetExplosionAt(struct Cadet* cadet, struct Vector3* origin, float explosionRadius) {
    struct Vector3 offset;
    vector3Sub(&cadet->transform.position, origin, &offset);

    float distSqr = vector3MagSqrd(&offset);

    if (distSqr < explosionRadius * explosionRadius) {
        offset.y += explosionRadius * explosionRadius;
        vector3Normalize(&offset, &offset);
        cadet->state = cadetFreefall;
        cadet->actor.velocity.x = offset.x * CADET_SPEED * 2.0f;
        cadet->actor.velocity.y = offset.y * CADET_EXLODE_JUMP_IMPULSE;
        cadet->actor.velocity.z = offset.z * CADET_SPEED * 2.0f;
    }

}

void cadetTeleportIn(struct Cadet* cadet) {
    if (cadet->actor.lastStableAnchor) {
        transformPoint(cadet->actor.lastStableAnchor, &cadet->actor.lastStableLocation, &cadet->transform.position);
    }

    if (!teleportEffectUpdate(&gCadet.teleport)) {
        respawnEffectEnd(&cadet->respawn);
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
        dropShadowCalculate(&cadet->shadow, 0, 0);
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

void cadetCheckForSwamp(struct Cadet* cadet) {
    if (gLevelCollisionGrid && !splashEffectActive(&cadet->splash)) {
        struct Vector3 raycastCheck;
        raycastCheck.x = cadet->transform.position.x;
        raycastCheck.y = 0.1f;
        raycastCheck.z = cadet->transform.position.z;
        struct Vector3 raycastDir;
        raycastDir.x = 0.0f;
        raycastDir.y = -1.0f;
        raycastDir.z = 0.0f;
        struct ContactPoint contact;
        if (collisionGridRaycast(gLevelCollisionGrid, &raycastCheck, &raycastDir, CollisionLayersSwamp, 1.0f, &contact) != RAYCAST_NO_HIT) {
            splashEffectInit(&cadet->splash, &contact.point, fabsf(cadet->actor.velocity.y) * 0.1f);
        }
    }
}

void cadetUpdate(void* cadetPtr) {
    struct Cadet* cadet = (struct Cadet*)cadetPtr;
    if (cadet->actor.stateFlags & CADET_IS_INVISIBLE) {
        return;
    }

    float prevY = cadet->transform.position.y;

    cadet->state(cadet);

    cadetUpdatefootstepSound(cadet);

    if (prevY >= SPLASH_CHECK_THRESH && cadet->transform.position.y < SPLASH_CHECK_THRESH) {
        cadetCheckForSwamp(cadet);
    }

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
    dropShadowInit(&gCadet.shadow, &gCadet.transform, &gCadetShadowParams);

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
    gCadet.controllerIndex = -1;

    gCadet.respawn.renderId = -1;

    dynamicActorAddToGroup(&gScene.dynamicActors, &gCadet.transform, &gCadet, cadetRender, MATERIAL_INDEX_NOT_BATCHED, 0.5f);
}

void cadetInit() {
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