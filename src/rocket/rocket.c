
#include "rocket.h"

#include "src/rocket/geo/model.h"

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