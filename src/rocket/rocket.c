
#include "rocket.h"

#include "src/rocket/geo/model.h"
#include "src/graphics/renderscene.h"
#include "src/collision/collisionscene.h"
#include "src/collision/geo/rocket_collision.inc.c"
#include "src/collision/collisiondata.h"

#define LANDING_TIME    3.0f
#define LANDING_ACCEL   2.0f
#define LAUNCH_DELAY    2.0f

struct Rocket gRocket;

struct RocketTrailParameters gRocketTrailParameters = {
    TransparentMaterialTypeShockwave,
    {0.0f, 1.0f, 0.0f}, 
    -2.0f,
    0.5f,
    0.1f,
    1.0f,
    0.5f,
};

void rocketRender(struct DynamicActor* data, struct GraphicsState* state) {
    struct Rocket* rocket = (struct Rocket*)data->data;

    Mtx* nextTransfrom = graphicsStateNextMtx(state);
    Mtx* reflectMatrix = graphicsStateNextMtx(state);

    LookAt* nextLookat = graphicsStateNextLookat(state);

    guLookAtReflect(reflectMatrix, nextLookat, 
        state->cameraPos->x,
        state->cameraPos->y,
        state->cameraPos->z,
        rocket->transform.position.x,
        rocket->transform.position.y,
        rocket->transform.position.z,
        0,
        1,
        0
    );

    transformToMatrixL(data->transform, 1.0f / 256.0f, nextTransfrom);

    gSPLookAt(state->dl++, K0_TO_PHYS(nextLookat));

    gSPMatrix(state->dl++, OS_K0_TO_PHYSICAL(nextTransfrom), G_MTX_MODELVIEW|G_MTX_MUL|G_MTX_PUSH);
    gDPSetEnvColor(state->dl++, rocket->color.r, rocket->color.g, rocket->color.b, rocket->color.a);
    gSPDisplayList(state->dl++, OS_K0_TO_PHYSICAL(Rocket_Rocket_mesh));
    gSPPopMatrix(state->dl++, G_MTX_MODELVIEW);
}

void rocketUpdate(void* data) {
    struct Rocket* rocket = (struct Rocket*)data;

    if (rocket->animationTiming >= 0.0f) {
        vector3AddScaled(&rocket->landingSpot, &gUp, rocket->animationTiming*rocket->animationTiming*LANDING_ACCEL, &rocket->transform.position);
    }
    
    if (rocket->rocketFlags & ROCKET_FLAGS_LAUNCHING) {
        rocket->animationTiming += gTimeDelta;

        if (rocket->animationTiming >= 0.0f) {
            rocket->rocketFlags = ROCKET_FLAGS_ANIMATION_DONE;
            timeRemoveListener(&rocket->updateListener, TimeUpdateGroupWorld);
            rocketTrailStop(&rocket->trail);
        }
    } else {
        rocket->animationTiming -= gTimeDelta;

        if (rocket->animationTiming <= 0.0f) {
            rocket->transform.position = rocket->landingSpot;
            timeRemoveListener(&rocket->updateListener, TimeUpdateGroupWorld);
            rocketTrailStop(&rocket->trail);
        }
    }
}

void rocketLandAt(struct Rocket* rocket, struct Vector3* location) {
    rocketStartAt(rocket, location);
    rocket->animationTiming = LANDING_TIME;
    timeAddListener(&rocket->updateListener, rocketUpdate, rocket, TimeUpdateGroupWorld);
    vector3AddScaled(location, &gUp, LANDING_TIME*LANDING_TIME*LANDING_ACCEL, &rocket->transform.position);

    rocketTrailStart(&rocket->trail, &rocket->transform, &gRocketTrailParameters, ROCKET_TRAIL_PARAMETER_COUNT, ROCKET_TRAIL_FLAGS_RELATIVE);
}

void rocketStartAt(struct Rocket* rocket, struct Vector3* location) {
    rocket->color.r = 255;
    rocket->color.g = 255;
    rocket->color.b = 255;
    rocket->color.a = 255;

    rocket->landingSpot = *location;
    quatIdent(&rocket->transform.rotation);
    rocket->transform.scale = 1.0f;
    rocket->transform.position = *location;
    rocket->animationTiming = 0.0f;
    rocket->rocketFlags = 0;

    dynamicActorAddToGroup(&gScene.dynamicActors, &rocket->transform, rocket, rocketRender, MATERIAL_INDEX_NOT_BATCHED, 4.3f);

    rocket->collider.collider = &_rocket_collision_collider;
    rocket->collider.transform = &rocket->transform;
    rocket->collider.triggerMask = 0;
    rocket->collider.data = 0;
    rocket->collider.trigger = 0;

    sparseCollisionAdd(&gSparseCollisionGrid, &rocket->collider, NULL);
}

void rocketLaunch(struct Rocket* rocket) {
    rocket->rocketFlags |= ROCKET_FLAGS_LAUNCHING;
    rocket->animationTiming = -LAUNCH_DELAY;
    timeAddListener(&rocket->updateListener, rocketUpdate, rocket, TimeUpdateGroupWorld);
}