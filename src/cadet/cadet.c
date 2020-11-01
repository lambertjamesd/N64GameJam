
#include "cadet.h"
#include "src/input/controller.h"
#include "src/graphics/renderscene.h"

struct Cadet gCadet;

void cadetMove(struct Cadet* cadet) {
    cadet->transform.position.x += gTimeDelta * gControllerState[0].stick_x / 80.0f;
    cadet->transform.position.z -= gTimeDelta * gControllerState[0].stick_y / 80.0f;
}

void cadetUpdate(void* cadetPtr) {
    struct Cadet* cadet = (struct Cadet*)cadetPtr;
    cadet->state(cadet);

    gScene.camera.targetPosition = cadet->transform.position;
}

void cadetReset(struct Vector3* startLocation) {
    transformIdentity(&gCadet.transform);
    gCadet.transform.position = *startLocation;
    gCadet.state = cadetMove;

    gCadet.velocity = gZeroVec;
    gCadet.lastStableLocation = *startLocation;
    gCadet.isJumping = 0;
    gCadet.anchor = 0;
    gCadet.relativeToAnchor = gZeroVec;
}

void cadetInit() {
    timeAddListener(&gCadet.updateListener, cadetUpdate, &gCadet);
    gCadet.transform.scale = 1.0f / 256.0f;

    cadetReset(&gZeroVec);
}