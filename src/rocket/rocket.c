
#include "rocket.h"

#include "src/rocket/geo/model.h"
#include "src/graphics/renderscene.h"

#define LANDING_TIME    3.0f
#define LANDING_ACCEL   2.0f

struct Rocket gRocket;

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
    gSPDisplayList(state->dl++, Rocket_Rocket_mesh);
    gSPPopMatrix(state->dl++, G_MTX_MODELVIEW);
}

void rocketUpdate(void* data) {
    struct Rocket* rocket = (struct Rocket*)data;

    vector3AddScaled(&rocket->landingSpot, &gUp, rocket->animationTiming*rocket->animationTiming*LANDING_ACCEL, &rocket->transform.position);
    rocket->animationTiming -= gTimeDelta;

    if (rocket->animationTiming <= 0.0f) {
        rocket->transform.position = rocket->landingSpot;
        timeRemoveListener(&rocket->updateListener, TimeUpdateGroupWorld);
    }
}

void rocketLandAt(struct Rocket* rocket, struct Vector3* location) {
    timeAddListener(&rocket->updateListener, rocketUpdate, rocket, TimeUpdateGroupWorld);

    rocket->color.r = 255;
    rocket->color.g = 255;
    rocket->color.b = 255;
    rocket->color.a = 255;

    rocket->landingSpot = *location;
    quatIdent(&rocket->transform.rotation);
    rocket->transform.scale = 1.0f;
    vector3AddScaled(location, &gUp, LANDING_TIME*LANDING_TIME*LANDING_ACCEL, &rocket->transform.position);
    rocket->animationTiming = LANDING_TIME;

    dynamicActorAddToGroup(&gScene.dynamicActors, &rocket->transform, rocket, rocketRender, MATERIAL_INDEX_NOT_BATCHED, 4.3f);
}