
#include "switch.h"
#include "src/collision/collisionscene.h"
#include "src/graphics/renderscene.h"
#include "src/level/level.h"
#include "src/audio/playersounds.h"
#include "src/audio/audio.h"
#include "src/puzzle/geo/permanantswitch.inc.c"
#include "src/collision/geo/permanant_switch.inc.c"
#include "src/collision/geo/large_switch.inc.c"

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
        {-0.2f, 0.0f, -0.2f},
        {0.2f, 0.25f, 0.2f},
    }
};

struct Quaternion gRotateSwitch45 = {
    0.0f,
    0.382683432f,
    0.0f,
    0.923879533f,
};

#define HIDE_TIME   3.0f
#define HIDE_SPEED  0.25f

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

    float scale = 1.0f / 256.0f;
    
    switch (puzzleSwitch->switchType) {
        case PuzzleSwitchTypeLarge:
            toRender = gLoadedLevel->theme->theme->themeMeshes[LevelThemeMeshesLSwitchUp + puzzleSwitch->sender.currentSignal];
            break;
        case PuzzleSwitchTypeSmall:
            toRender = gLoadedLevel->theme->theme->themeMeshes[LevelThemeMeshesSSwitchUp + puzzleSwitch->sender.currentSignal];
            break;
        case PuzzleSwitchTypeLargePermanant:
            toRender = PermanantSwitch_PermanantSwitch_mesh_tri_0;
            break;
        case PuzzleSwitchTypeSmallPermanant:
            toRender = PermanantSwitch_PermanantSwitch_mesh_tri_0;
            scale = 1.0f / 512.0f;
            break;
    }

    Mtx* nextTransfrom = graphicsStateNextMtx(state);

    gDPPipeSync(state->dl++);

    if (puzzleSwitch->sender.currentSignal) {
        graphicsStateSetPrimitiveColor(state, gSwitchColors[puzzleSwitch->sender.signalIndex]);
    } else {
        graphicsStateSetPrimitiveColor(state, gSwitchDarkColors[puzzleSwitch->sender.signalIndex]);
    }


    transformToMatrixL(data->transform, scale, nextTransfrom);

    gSPMatrix(state->dl++, OS_K0_TO_PHYSICAL(nextTransfrom), G_MTX_MODELVIEW|G_MTX_MUL|G_MTX_PUSH);
    gSPDisplayList(state->dl++, toRender);
    gDPPipeSync(state->dl++);
    gSPPopMatrix(state->dl++, G_MTX_MODELVIEW);
}

void switchTrigger(void* data, struct Vector3* origin) {
    struct PuzzleSwitch* puzzleSwitch = (struct PuzzleSwitch*)data;

    if (!puzzleSwitch->didTrigger) {
        switchTriggerSound(puzzleSwitch);
    }

    if (!(puzzleSwitch->switchType & PUZZLE_SWITCH_PERMANANT) && puzzleSwitch->collider.collider) {
        puzzleSwitch->collider.collider->collisionMask = 0;
    }

    puzzleSwitch->didTrigger = 2;
}

void switchUpdate(void* data) {
    struct PuzzleSwitch* puzzleSwitch = (struct PuzzleSwitch*)data;
    signalSetSender(&puzzleSwitch->sender, puzzleSwitch->didTrigger != 0);

    if (puzzleSwitch->didTrigger > 0) {
        if (puzzleSwitch->switchType & PUZZLE_SWITCH_PERMANANT) {
            puzzleSwitch->animTimer += gTimeDelta;

            if (puzzleSwitch->animTimer > HIDE_TIME) {
                timeRemoveListener(&puzzleSwitch->updateListener, TimeUpdateGroupWorld);
                sparseCollisionRemove(&gSparseCollisionGrid, &puzzleSwitch->trigger);
            } else {
                puzzleSwitch->transform.position.y -= gTimeDelta * HIDE_SPEED;
                
                if (puzzleSwitch->switchType == PuzzleSwitchTypeLarge) {
                    sparseCollisionReindex(&gSparseCollisionGrid, &puzzleSwitch->collider, 0);
                }
            }
        } else {
            puzzleSwitch->didTrigger--;

            if (!puzzleSwitch->didTrigger) {
                switchTriggerSound(puzzleSwitch);

                if (puzzleSwitch->collider.collider) {
                    puzzleSwitch->collider.collider->collisionMask = CollisionLayersCadetSwitch;
                }
            }
        }
    }
}

void switchInit(struct PuzzleSwitch* puzzleSwitch, struct Vector3* position, enum PuzzleSwitchType type, int color) {
    struct CollisionCollider* trigger;
    struct CollisionCollider* collider = 0;
    int triggerMask = 0;

    switch (type) {
        case PuzzleSwitchTypeLarge:
            trigger = &gLargeSwitchCollider;
            triggerMask = CollisionLayersRobotSwitch;
            collider = &_large_switch_collider;
            break;
        case PuzzleSwitchTypeLargePermanant:
            trigger = &gLargeSwitchCollider;
            triggerMask = CollisionLayersRobotSwitch;
            collider = &_permanant_switch_collider;
            break;
        default:
            trigger = &gSmallSwitchCollider;
            triggerMask = CollisionLayersMutualSwitch;
            break;
    }
    
    transformIdentity(&puzzleSwitch->transform);
    puzzleSwitch->transform.position = *position;

    puzzleSwitch->trigger.collider = trigger;
    puzzleSwitch->trigger.transform = &puzzleSwitch->transform;
    puzzleSwitch->trigger.data = puzzleSwitch;
    puzzleSwitch->trigger.trigger = switchTrigger;
    puzzleSwitch->trigger.triggerMask = triggerMask;

    if (collider) {
        puzzleSwitch->collider.collider = collider;
        puzzleSwitch->collider.transform = &puzzleSwitch->transform;
        puzzleSwitch->collider.data = 0;
        puzzleSwitch->collider.trigger = 0;
        puzzleSwitch->collider.triggerMask = 0;
        sparseCollisionAdd(&gSparseCollisionGrid, &puzzleSwitch->collider, NULL);
    } else {
        puzzleSwitch->collider.collider = 0;
        puzzleSwitch->collider.transform = 0;
        puzzleSwitch->collider.data = 0;
        puzzleSwitch->collider.trigger = switchTrigger;
    }

    puzzleSwitch->sender.currentSignal = 0;
    puzzleSwitch->sender.signalIndex = color;

    timeAddListener(&puzzleSwitch->updateListener, switchUpdate, puzzleSwitch, TimeUpdateGroupWorld);

    puzzleSwitch->didTrigger = 0;
    puzzleSwitch->switchType = type;
    puzzleSwitch->animTimer = 0.0f;

    sparseCollisionAdd(&gSparseCollisionGrid, &puzzleSwitch->trigger, NULL);

    dynamicActorAddToGroup(&gScene.dynamicActors, &puzzleSwitch->transform, puzzleSwitch, switchRender, DynamicMaterialTypeSwitch, 1.4f);
}