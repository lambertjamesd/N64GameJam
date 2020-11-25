
#include "rocket.h"

#include "src/rocket/geo/model.h"

struct Rocket gRocket;

void rocketRender(struct DynamicActor* data, struct GraphicsState* state) {
    struct Rocket* robot = (struct Rocket*)data->data;

    Mtx* nextTransfrom = graphicsStateNextMtx(state);
    Mtx* reflectMatrix = graphicsStateNextMtx(state);

    LookAt* nextLookat = graphicsStateNextLookat(state);

    guLookAtReflect(reflectMatrix, nextLookat, 
        state->cameraPos->x,
        state->cameraPos->y,
        state->cameraPos->z,
        robot->transform.position.x,
        robot->transform.position.y,
        robot->transform.position.z,
        0,
        1,
        0
    );

    transformToMatrixL(data->transform, 1.0f / 256.0f, nextTransfrom);

    gSPLookAt(state->dl++, K0_TO_PHYS(nextLookat));

    gSPMatrix(state->dl++, OS_K0_TO_PHYSICAL(nextTransfrom), G_MTX_MODELVIEW|G_MTX_MUL|G_MTX_PUSH);
    gSPDisplayList(state->dl++, Rocket_Rocket_mesh);
    gSPPopMatrix(state->dl++, G_MTX_MODELVIEW);
}