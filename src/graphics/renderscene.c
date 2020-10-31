
#include "renderscene.h"

#include "src/math/mathf.h"

struct SceneCamera gMainCamera;

void cameraCalculatePos(struct Quaternion* rotation, struct Vector3* target, float distance, struct Vector3* out) {
    quatMultVector(rotation, &gForward, out);

    out->x = out->x * distance + target->x;
    out->y = out->y * distance + target->y;
    out->z = out->z * distance + target->z;
}

void cameraUpdate(void* cameraPtr) {
    struct SceneCamera* camera = cameraPtr;

    float moveAmount = CAMERA_MAX_SPEED * gTimeDelta;

    vector3MoveTowards(&camera->centerPosition, &camera->targetPosition, moveAmount, &camera->centerPosition);
    camera->followDistance = mathfMoveTowards(camera->followDistance, camera->targetFollowDistance, moveAmount);
    cameraCalculatePos(&camera->transform.rotation, &camera->centerPosition, camera->followDistance, &camera->transform.position);
}


void cameraInit(struct SceneCamera* camera) {
    quatAxisAngle(&gRight, M_PI / 3.0f, &camera->transform.rotation);
    camera->targetPosition = camera->centerPosition = gZeroVec;
    camera->transform.scale = 1.0f;
    camera->followDistance = camera->targetFollowDistance = START_FOLLOW_DISTANCE;

    cameraCalculatePos(&camera->transform.rotation, &camera->targetPosition, camera->followDistance, &camera->transform.position);

    timeAddListener(&camera->updateListener, cameraUpdate, camera);
}

void cameraCleanup(struct SceneCamera* camera) {
    timeRemoveListener(&camera->updateListener);
}