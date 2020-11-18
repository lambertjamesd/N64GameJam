
#include "entranceexit.h"
#include "src/collision/geo/geo.h"
#include "src/collision/collisionscene.h"
#include "src/graphics/renderscene.h"
#include "src/level/level.h"
#include "src/math/mathf.h"
#include "src/audio/playersounds.h"
#include "src/audio/audio.h"

struct CollisionCollider gExitTriggerCollider = {
    ColliderTypeBox,
    0,
    .box = {
        {-0.05f, 0.0f, -0.05f},
        {0.05f, 0.5f, 0.05f},
    }
};

struct CollisionCollider gExitTriggerSmallCollider = {
    ColliderTypeBox,
    0,
    .box = {
        {-0.3f, 0.0f, -0.3f},
        {0.3f, 0.5f, 0.3f},
    }
};

void exitEntranceRender(struct DynamicActor* data, struct GraphicsState* state) {
    struct EntranceExit* entranceExit = (struct EntranceExit*)data->data;
    Gfx* toRender = gLoadedLevel->theme->theme->themeMeshes[LevelThemeMeshesEntranceExit];

    Mtx* nextTransfrom = graphicsStateNextMtx(state);

    if (entranceExit->isActive || (gLevelFlags & LEVEL_EXIT_CUTSCENE)) {
        gSPDisplayList(state->dl++, gLoadedLevel->theme->theme->dynamicMaterials[DynamicMaterialTypeEntranceExit]);
    } else {
        gSPDisplayList(state->dl++, gLoadedLevel->theme->theme->dynamicMaterials[DynamicMaterialTypeEntranceExitOff]);
    }

    transformToMatrixL(data->transform, 1.0f / 256.0f, nextTransfrom);
    gSPMatrix(state->dl++, OS_K0_TO_PHYSICAL(nextTransfrom), G_MTX_MODELVIEW|G_MTX_MUL|G_MTX_PUSH);
    gSPDisplayList(state->dl++, toRender);
    gSPPopMatrix(state->dl++, G_MTX_MODELVIEW);
    gSPDisplayList(state->dl++, gLoadedLevel->theme->theme->dynamicMaterialCleanup[DynamicMaterialTypeEntranceExit]);
}

void entraceExitTrigger(void* data) {
    struct EntranceExit* exit = (struct EntranceExit*)data;

    if (!exit->isActive) {
        audioPlaySound(
            gPlayerSoundIds[(exit->exitTrigger.triggerMask & CollisionLayersCadetSwitch) ?
                GoalTouchSmall : GoalTouchBig],
            0.5f,
            1.0f,
            0.0f,
            10
        );
    }

    exit->isActive = 2;
}

void entranceExitUpdate(void* data) {
    struct EntranceExit* exit = (struct EntranceExit*)data;
    if (exit->isActive) {
        exit->isActive--;
    }
}

void entranceExitInit(struct EntranceExit* exit, struct Vector3* pos, int isCadet) {
    transformIdentity(&exit->transform);
    exit->transform.position = *pos;
    
    if (isCadet) {
        exit->transform.scale = 0.5f;
        exit->exitTrigger.collider = &gExitTriggerSmallCollider;
    } else {
        exit->transform.scale = 1.0f;
        exit->exitTrigger.collider = &gExitTriggerCollider;
    }

    timeAddListener(&exit->updateListener, entranceExitUpdate, exit);
    
    exit->collider.collider = &_entrance_exit_collider;
    exit->collider.transform = &exit->transform;
    exit->collider.data = 0;
    exit->collider.trigger = 0;
    exit->collider.triggerMask = 0;

    struct CollisionBox bb;
    bb.min.x = pos->x - 0.8f;
    bb.min.y = pos->y;
    bb.min.z = pos->z - 0.8f;

    bb.max.x = pos->x + 0.8f;
    bb.max.y = pos->y + 0.5f;
    bb.max.z = pos->z + 0.8f;
    sparseCollisionReindex(&gSparseCollisionGrid, &exit->collider, &bb, 0);

    exit->exitTrigger.transform = &exit->transform;
    exit->exitTrigger.data = exit;
    exit->exitTrigger.trigger = entraceExitTrigger;
    exit->exitTrigger.triggerMask = isCadet ? CollisionLayersCadetSwitch : CollisionLayersRobotSwitch;

    bb.min.x = pos->x - 1.0f;
    bb.min.y = pos->y;
    bb.min.z = pos->z - 1.0f;

    bb.max.x = pos->x + 1.0f;
    bb.max.y = pos->y + 0.15f;
    bb.max.z = pos->z + 1.0f;
    sparseCollisionReindex(&gSparseCollisionGrid, &exit->exitTrigger, &bb, 0);

    dynamicActorAddToGroup(&gScene.dynamicActors, &exit->transform, exit, exitEntranceRender, MATERIAL_INDEX_NOT_BATCHED);
}

struct EntranceExit gCadetExit;
struct EntranceExit gRobotExit;