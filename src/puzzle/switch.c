
#include "switch.h"
#include "src/collision/collisionscene.h"
#include "src/graphics/renderscene.h"
#include "src/level/level.h"
#include "src/audio/playersounds.h"
#include "src/audio/audio.h"

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

struct Quaternion gRotateSwitch45 = {
    0.0f,
    0.382683432f,
    0.0f,
    0.923879533f,
};

void switchTriggerSound(struct PuzzleSwitch* puzzleSwitch) {
    int sound;
    switch (puzzleSwitch->switchType) {
        case PuzzleSwitchTypeLarge:
        case PuzzleSwitchTypeLargePermanant:
            sound = ButtonSoundsBig;
            break;
        default:
            sound = ButtonSoundsSmall;
            break;
    }
    audioPlaySound(
        gPlayerSoundIds[sound],
        0.5f,
        1.0f,
        0.0f,
        10
    );
}

void switchRender(struct DynamicActor* data, struct GraphicsState* state) {
    struct PuzzleSwitch* puzzleSwitch = (struct PuzzleSwitch*)data->data;
    Gfx* toRender;
    
    switch (puzzleSwitch->switchType) {
        case PuzzleSwitchTypeLarge:
        case PuzzleSwitchTypeLargePermanant:
            toRender = gLoadedLevel->theme->theme->themeMeshes[LevelThemeMeshesLSwitchUp + puzzleSwitch->sender.currentSignal];
            break;
        default:
            toRender = gLoadedLevel->theme->theme->themeMeshes[LevelThemeMeshesSSwitchUp + puzzleSwitch->sender.currentSignal];
            break;
    }

    Mtx* nextTransfrom = graphicsStateNextMtx(state);

    if (puzzleSwitch->sender.currentSignal) {
        graphicsStateSetPrimitiveColor(state, gSwitchColors[puzzleSwitch->sender.signalIndex]);
    } else {
        graphicsStateSetPrimitiveColor(state, gSwitchDarkColors[puzzleSwitch->sender.signalIndex]);
    }


    if (puzzleSwitch->switchType == PuzzleSwitchTypeLargePermanant || 
        puzzleSwitch->switchType == PuzzleSwitchTypeSmallPermanant) {
        struct BasicTransform finalTransform;
        finalTransform.position = data->transform->position;
        finalTransform.scale = data->transform->scale;
        finalTransform.rotation = gRotateSwitch45;
        transformToMatrixL(&finalTransform, 1.0f / 256.0f, nextTransfrom);
    } else {
        transformToMatrixL(data->transform, 1.0f / 256.0f, nextTransfrom);
    }

    gSPMatrix(state->dl++, OS_K0_TO_PHYSICAL(nextTransfrom), G_MTX_MODELVIEW|G_MTX_MUL|G_MTX_PUSH);
    gSPDisplayList(state->dl++, toRender);
    gSPPopMatrix(state->dl++, G_MTX_MODELVIEW);
}

void switchTrigger(void* data, struct Vector3* origin) {
    struct PuzzleSwitch* puzzleSwitch = (struct PuzzleSwitch*)data;

    if (!puzzleSwitch->didTrigger) {
        switchTriggerSound(puzzleSwitch);
    }

    puzzleSwitch->didTrigger = 2;
}

void switchUpdate(void* data) {
    struct PuzzleSwitch* puzzleSwitch = (struct PuzzleSwitch*)data;
    signalSetSender(&puzzleSwitch->sender, puzzleSwitch->didTrigger != 0);

    if (!(puzzleSwitch->switchType & PUZZLE_SWITCH_PERMANANT) && puzzleSwitch->didTrigger > 0) {
        puzzleSwitch->didTrigger--;

        if (!puzzleSwitch->didTrigger) {
            switchTriggerSound(puzzleSwitch);
        }
    }
}

void switchInit(struct PuzzleSwitch* puzzleSwitch, struct Vector3* position, enum PuzzleSwitchType type, int color) {
    struct CollisionCollider* collider;
    int triggerMask = 0;

    switch (type) {
        case PuzzleSwitchTypeLarge:
        case PuzzleSwitchTypeLargePermanant:
            collider = &gLargeSwitchCollider;
            triggerMask = CollisionLayersRobotSwitch;
            break;
        default:
            collider = &gSmallSwitchCollider;
            triggerMask = CollisionLayersMutualSwitch;
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

    timeAddListener(&puzzleSwitch->updateListener, switchUpdate, puzzleSwitch, TimeUpdateGroupWorld);

    puzzleSwitch->didTrigger = 0;
    puzzleSwitch->switchType = type;

    struct CollisionBox bb;
    vector3Add(&collider->box.min, position, &bb.min);
    vector3Add(&collider->box.max, position, &bb.max);
    sparseCollisionReindex(&gSparseCollisionGrid, &puzzleSwitch->collider, &bb, 0);

    dynamicActorAddToGroup(&gScene.dynamicActors, &puzzleSwitch->transform, puzzleSwitch, switchRender, DynamicMaterialTypeSwitch, 1.0f);
}