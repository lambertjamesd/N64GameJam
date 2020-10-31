
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

void cadetInit() {
    transformIdentity(&gCadet.transform);
    gCadet.transform.scale = 1.0f / 256.0f;
    gCadet.state = cadetMove;
    timeAddListener(&gCadet.updateListener, cadetUpdate, &gCadet);
}