
#include "breakable.h"

#include "src/collision/collisionscene.h"
#include "src/graphics/renderscene.h"
#include "src/level/level.h"

struct CollisionCollider gBreakablePlatformCollider = {
    ColliderTypeBox,
    CollisionLayersGeometry,
    .box = {
        {-1.0f, 0.0f, -1.0f},
        {1.0f, 2.0f, 1.0f},
    }
};

struct CollisionCollider gBreakableBarrierCollider = {
    ColliderTypeBox,
    CollisionLayersGeometry,
    .box = {
        {-1.0f, 0.0f, -1.0f},
        {1.0f, 4.0f, 1.0f},
    }
};

void breakableTrigger(void* data, struct Vector3* origin) {
    struct Breakable* breakable = (struct Breakable*)data;
    if (breakable->renderActorId != ACTOR_ID_NONE) {
        dynamicActorRemoveFromGroup(&gScene.dynamicActors, &breakable->renderActorId);

        struct Box bb;
        vector3Add(&breakable->collider.collider->box.min, &breakable->transform.position, &bb.min);
        vector3Add(&breakable->collider.collider->box.max, &breakable->transform.position, &bb.max);
        sparseCollisionReindex(&gSparseCollisionGrid, &breakable->collider, 0, &bb);

        rockFragmentsInit(&breakable->breakEffect, &breakable->transform.position, breakable->type ? 3 : 2, origin);
    }
}

void breakableRender(struct DynamicActor* data, struct GraphicsState* state) {
    struct Breakable* breakable = (struct Breakable*)data->data;
    Gfx* toRender = gLoadedLevel->theme->theme->themeMeshes[
        breakable->type ? LevelThemeMeshesBreakBarrier : LevelThemeMeshesBreakPlatform
    ];

    Mtx* nextTransfrom = graphicsStateNextMtx(state);
    transformToMatrixL(data->transform, 1.0f / 256.0f, nextTransfrom);
    gSPMatrix(state->dl++, OS_K0_TO_PHYSICAL(nextTransfrom), G_MTX_MODELVIEW|G_MTX_MUL|G_MTX_PUSH);
    gSPDisplayList(state->dl++, toRender);
    gSPPopMatrix(state->dl++, G_MTX_MODELVIEW);
    gDPPipeSync(state->dl++);
}

void breakableInit(struct Breakable* breakable, struct Vector3* position, int type) {
    transformIdentity(&breakable->transform);
    breakable->transform.position = *position;

    struct CollisionCollider* collider;

    switch (type) {
        case BreakableTypePlatform:
            collider = &gBreakablePlatformCollider;
            break;
        default:
            collider = &gBreakableBarrierCollider;
            break;
    }

    breakable->collider.collider = collider;
    breakable->collider.transform = &breakable->transform;
    breakable->collider.data = breakable;
    breakable->collider.trigger = breakableTrigger;
    breakable->collider.triggerMask = CollisionLayersBreakable;

    breakable->type = type;

    struct Box bb;
    vector3Add(&collider->box.min, position, &bb.min);
    vector3Add(&collider->box.max, position, &bb.max);
    sparseCollisionReindex(&gSparseCollisionGrid, &breakable->collider, &bb, 0);

    breakable->renderActorId = dynamicActorAddToGroup(&gScene.dynamicActors, &breakable->transform, breakable, breakableRender, DynamicMaterialTypeBreakable, 2.0f);

}