
#include "renderscene.h"

#include "src/math/mathf.h"
#include "src/input/controller.h"
#include "src/input/inputfocus.h"
#include "src/defs.h"

#define SPLIT_MOVE_SPEED    2

struct RenderScene gScene;

short gTargetSplit;
short gCurrentSplit;

#define FOLLOW_DISTANCE_START 0
#define CAMERA_ROTATE_STEP (M_PI * 0.25f)

float gFollowDistances[] = {
    5.0f,
    10.0f,
    16.0f,
};

float gFollowPitches[] = {
    -M_PI / 3.5f,
    -M_PI / 3.0f,
    -M_PI / 2.6f,
};

#define FOLLOW_STEP_COUNT (sizeof(gFollowDistances) / sizeof(float))

void viewportConvert(struct SceneViewport* input, Vp* output) {
    output->vp.vscale[0] = (input->maxx - input->minx) * 2;
    output->vp.vscale[1] = (input->maxy - input->miny) * 2;
    output->vp.vscale[2] = G_MAXZ/2;
    output->vp.vscale[3] = 0;

    output->vp.vtrans[0] = (input->maxx + input->minx) * 2;
    output->vp.vtrans[1] = (input->maxy + input->miny) * 2;
    output->vp.vtrans[2] = G_MAXZ/2;
    output->vp.vtrans[3] = 0;
}

void cameraCalculatePos(struct Quaternion* rotation, struct Vector3* target, float distance, struct Vector3* out) {
    quatMultVector(rotation, &gForward, out);

    out->x = out->x * distance + target->x;
    out->y = out->y * distance + target->y;
    out->z = out->z * distance + target->z;
}

void cameraUpdate(void* cameraPtr) {
    struct SceneCamera* camera = cameraPtr;

    if (gInputMask & InputMaskCamera) {
        if (getButtonDown(camera->controllerIndex, D_CBUTTONS)) {
            if (camera->followDistanceStep + 1 < FOLLOW_STEP_COUNT) {
                ++camera->followDistanceStep;
            }
        }

        if (getButtonDown(camera->controllerIndex, U_CBUTTONS)) {
            if (camera->followDistanceStep > 0) {
                --camera->followDistanceStep;
            }
        }

        if (getButtonDown(camera->controllerIndex, R_CBUTTONS)) {
            camera->targetRotation += CAMERA_ROTATE_STEP;
        }

        if (getButtonDown(camera->controllerIndex, L_CBUTTONS)) {
            camera->targetRotation -= CAMERA_ROTATE_STEP;
        }
    }

    float moveAmount = CAMERA_MAX_SPEED * gTimeDelta;

    struct Quaternion yaw;
    struct Quaternion pitch;
    quatAxisAngle(&gRight, camera->currentPitch, &pitch);
    quatAxisAngle(&gUp, camera->currentRotation, &yaw);
    quatMultiply(&yaw, &pitch, &camera->transform.rotation);

    vector3Lerp(&camera->centerPosition, &camera->targetPosition, 0.1f, &camera->centerPosition);
    camera->followDistance = mathfLerp(camera->followDistance, gFollowDistances[camera->followDistanceStep], 0.1f);
    camera->currentRotation = mathfLerp(camera->currentRotation, camera->targetRotation, 0.1f);
    camera->currentPitch = mathfLerp(camera->currentPitch, gFollowPitches[camera->followDistanceStep], 0.1f);
    cameraCalculatePos(&camera->transform.rotation, &camera->centerPosition, camera->followDistance, &camera->transform.position);
}

void cameraSetFollowDistance(struct SceneCamera* camera, int followDistance) {
    camera->followDistanceStep = followDistance;
    camera->followDistance = gFollowDistances[followDistance];
    camera->currentPitch = gFollowPitches[followDistance];
}

void cameraInit(struct SceneCamera* camera, struct Vector3* startTarget, int controllerIndex) {
    quatAxisAngle(&gRight, -M_PI / 3.0f, &camera->transform.rotation);
    camera->targetPosition = camera->centerPosition = *startTarget;
    camera->transform.scale = 1.0f;
    camera->followDistance = gFollowDistances[FOLLOW_DISTANCE_START];
    camera->currentPitch = gFollowPitches[FOLLOW_DISTANCE_START];
    camera->targetDistanceStep = FOLLOW_DISTANCE_START;

    camera->followDistanceStep = FOLLOW_DISTANCE_START;
    camera->currentRotation = 0.0f;
    camera->targetRotation = 0.0f;
    camera->controllerIndex = controllerIndex;

    cameraCalculatePos(&camera->transform.rotation, &camera->targetPosition, camera->followDistance, &camera->transform.position);

    timeAddListener(&camera->updateListener, cameraUpdate, camera, TimeUpdateGroupWorld);
}

void cameraCleanup(struct SceneCamera* camera) {
    timeRemoveListener(&camera->updateListener, TimeUpdateGroupWorld);
}

void cameraGetMoveDir(struct SceneCamera* camera, struct Vector2* input, struct Vector2* out) {
    struct Vector3 right;
    struct Vector3 forward;

    quatMultVector(&camera->transform.rotation, &gRight, &right);
    quatMultVector(&camera->transform.rotation, &gForward, &forward);

    right.y = 0.0f;
    forward.y = 0.0f;

    vector3Normalize(&right, &right);
    vector3Normalize(&forward, &forward);

    out->x = right.x * input->x + forward.x * input->y;
    out->y = right.z * input->x + forward.z * input->y;
}

void renderSceneReset(struct RenderScene* scene) {
    dynamicActorGroupReset(&scene->dynamicActors);
    dynamicActorGroupReset(&scene->transparentActors);

    int i;
    for (i = 0; i < MAX_MATERIAL_GROUPS; ++i) {
        scene->transparentMaterials[i] = 0;
        scene->transparentMaterialCleanup[i] = 0;
    }
}

void renderSceneSetViewportSplit(int target) {
    gCurrentSplit = target;

    if (target <= 0) {
        target = -SPLIT_SCREEN_BORDER_SIZE / 2;
    } else if (target >= SCREEN_WD) {
        target = SCREEN_WD + SPLIT_SCREEN_BORDER_SIZE / 2;
    }

    gScene.viewports[0].maxx = target - SPLIT_SCREEN_BORDER_SIZE / 2;
    gScene.viewports[1].minx = target + SPLIT_SCREEN_BORDER_SIZE / 2;
}

void renderSceneSetTargetViewportSplit(int target) {
    gTargetSplit = target;
}

void renderSceneUpdateSplit() {
    int offset = gTargetSplit - gCurrentSplit;

    if (offset == 0) {
        return;
    }

    int currSplit = gCurrentSplit;

    if (offset < -SPLIT_MOVE_SPEED) {
        currSplit -= SPLIT_MOVE_SPEED;
    } else if (offset > SPLIT_MOVE_SPEED) {
        currSplit += SPLIT_MOVE_SPEED;
    } else {
        currSplit = gTargetSplit;
    }

    renderSceneSetViewportSplit(currSplit);
}