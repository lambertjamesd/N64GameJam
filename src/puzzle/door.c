
#include "door.h"
#include "src/collision/collisionscene.h"
#include "src/graphics/renderscene.h"
#include "src/level/level.h"
#include "src/math/mathf.h"

#define DOOR_MOVE_SPEED 2.0f

struct CollisionCollider gDoorCollider = {
    ColliderTypeBox,
    CollisionLayersGeometry,
    .box = {
        {-1.0f, 0.0f, -1.0f},
        {1.0f, 2.0f, 1.0f},
    }
};

void doorRender(struct DynamicActor* data, struct GraphicsState* state) {
    struct PuzzleDoor* door = (struct PuzzleDoor*)data->data;
    Gfx* toRender = gLoadedLevel->theme->theme->themeMeshes[LevelThemeMeshesDoor];

    Mtx* nextTransfrom = graphicsStateNextMtx(state);

    if (gCurrentSignal[door->signalIndex] ^ door->inverted) {
        graphicsStateSetPrimitiveColor(state, gSwitchColors[door->signalIndex]);
    } else {
        graphicsStateSetPrimitiveColor(state, gSwitchDarkColors[door->signalIndex]);
    }

    transformToMatrixL(data->transform, 1.0f / 256.0f, nextTransfrom);
    gSPMatrix(state->dl++, OS_K0_TO_PHYSICAL(nextTransfrom), G_MTX_MODELVIEW|G_MTX_MUL|G_MTX_PUSH);
    gSPDisplayList(state->dl++, toRender);
    gSPPopMatrix(state->dl++, G_MTX_MODELVIEW);
    gDPPipeSync(state->dl++);
}

void doorUpdate(void* data) {
    struct PuzzleDoor* door = (struct PuzzleDoor*)data;

    door->transform.position.y = mathfMoveTowards(
        door->transform.position.y,
        door->closedPosition.y + ((gCurrentSignal[door->signalIndex] ^ door->inverted) ? -2.0f : 0.0f),
        DOOR_MOVE_SPEED * gTimeDelta
    );
}

void doorInit(struct PuzzleDoor* door, struct Vector3* position, int color, int inverted) {
    transformIdentity(&door->transform);
    door->transform.position = *position;

    door->collider.collider = &gDoorCollider;
    door->collider.transform = &door->transform;
    door->collider.data = 0;
    door->collider.trigger = 0;
    door->collider.triggerMask = 0;

    timeAddListener(&door->updateListener, doorUpdate, door, TimeUpdateGroupWorld);

    door->signalIndex = color;
    door->closedPosition = *position;
    door->inverted = inverted;

    door->transform.position.y = door->closedPosition.y + (door->inverted ? -2.0f : 0.0f);

    struct Box bb;
    vector3Add(&gDoorCollider.box.min, position, &bb.min);
    vector3Add(&gDoorCollider.box.max, position, &bb.max);
    sparseCollisionReindex(&gSparseCollisionGrid, &door->collider, &bb, 0);

    dynamicActorAddToGroup(&gScene.dynamicActors, &door->transform, door, doorRender, DynamicMaterialTypeDoor, 2.0f);
}