
#include "switch.h"
#include "src/collision/collisionscene.h"
#include "src/graphics/renderscene.h"
#include "src/level/level.h"

struct CollisionCollider gLargeSwitchCollider = {
    ColliderTypeBox,
    0,
    .box = {
        {-0.576787f, 0.0f, -0.576787f},
        {0.576787f, 0.25f, 0.576787f},
    }
};

struct CollisionCollider gSmallSwitchCollider = {
    ColliderTypeBox,
    0,
    .box = {
        {-0.1466f, 0.0f, -0.1466f},
        {0.1466f, 0.25f, 0.1466f},
    }
};

void switchRender(struct DynamicActor* data, struct GraphicsState* state) {
    struct PuzzleSwitch* puzzleSwitch = (struct PuzzleSwitch*)data->data;
    Gfx* toRender = gLoadedLevel->theme->theme->themeMeshes[
        (puzzleSwitch->switchType << 1) + puzzleSwitch->sender.currentSignal
    ];

    Mtx* nextTransfrom = graphicsStateNextMtx(state);

    if (puzzleSwitch->sender.currentSignal) {
        graphicsStateSetPrimitiveColor(state, gSwitchColors[puzzleSwitch->sender.signalIndex]);
    } else {
        graphicsStateSetPrimitiveColor(state, gSwitchDarkColors[puzzleSwitch->sender.signalIndex]);
    }

    transformToMatrixL(data->transform, 1.0f / 256.0f, nextTransfrom);
    gSPMatrix(state->dl++, OS_K0_TO_PHYSICAL(nextTransfrom), G_MTX_MODELVIEW|G_MTX_MUL|G_MTX_PUSH);
    gSPDisplayList(state->dl++, toRender);
    gSPPopMatrix(state->dl++, G_MTX_MODELVIEW);
}

void switchTrigger(void* data) {
    struct PuzzleSwitch* puzzleSwitch = (struct PuzzleSwitch*)data;
    puzzleSwitch->didTrigger = 1;
}

void switchUpdate(void* data) {
    struct PuzzleSwitch* puzzleSwitch = (struct PuzzleSwitch*)data;
    signalSetSender(&puzzleSwitch->sender, puzzleSwitch->didTrigger);
    puzzleSwitch->didTrigger = 0;
}

void switchInit(struct PuzzleSwitch* puzzleSwitch, struct Vector3* position, enum PuzzleSwitchType type, int color) {
    struct CollisionCollider* collider;
    int triggerMask = 0;

    switch (type) {
        case PuzzleSwitchTypeLarge:
            collider = &gLargeSwitchCollider;
            triggerMask = CollisionLayersLargeSwitch;
            break;
        default:
            collider = &gSmallSwitchCollider;
            triggerMask = CollisionLayersSmallSwitch;
            break;
    }
    
    transformIdentity(&puzzleSwitch->transform);
    puzzleSwitch->transform.position = *position;

    puzzleSwitch->collider.collider = collider;
    puzzleSwitch->collider.transform = &puzzleSwitch->transform;
    puzzleSwitch->collider.data = puzzleSwitch;
    puzzleSwitch->collider.trigger = switchTrigger;
    puzzleSwitch->collider.triggerMask = triggerMask;

    puzzleSwitch->sender.currentSignal = 0;
    puzzleSwitch->sender.signalIndex = color;

    timeAddListener(&puzzleSwitch->updateListener, switchUpdate, puzzleSwitch);

    puzzleSwitch->didTrigger = 0;
    puzzleSwitch->switchType = type;

    struct CollisionBox bb;
    vector3Add(&collider->box.min, position, &bb.min);
    vector3Add(&collider->box.max, position, &bb.max);
    sparseCollisionReindex(&gSparseCollisionGrid, &puzzleSwitch->collider, &bb, 0);

    dynamicActorAddToGroup(&gScene.dynamicActors, &puzzleSwitch->transform, puzzleSwitch, switchRender, DynamicMaterialTypeSwitch);
}

void switchDestroy(void* data) {
    struct PuzzleSwitch* puzzleSwitch = (struct PuzzleSwitch*)data;
    signalSetSender(&puzzleSwitch->sender, 0);
    timeRemoveListener(&puzzleSwitch->updateListener);
}