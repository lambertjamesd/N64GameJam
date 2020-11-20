
#include "gem.h"
#include "geo/gem.inc.c"
#include "src/collision/collisionscene.h"
#include "src/graphics/renderscene.h"

struct CollisionCollider gGemCollider = {
    ColliderTypeBox,
    0,
    .box = {
        {-0.1466f, 0.0f, -0.1466f},
        {0.1466f, 0.4f, 0.1466f},
    }
};

struct Quaternion gGemRotationPerFrame = {
    0,
    0.017452406,
    0,
    0.999847695,
};

void gemRender(struct DynamicActor* data, struct GraphicsState* state) {
    struct Gem* gem = (struct Gem*)data->data;

    if (gem->flags & GEM_FLAGS_PREVIOUSLY_COLLECTED) {
        gDPSetEnvColor(state->dl++, 0, 0, 0, 98);
    } else {
        gDPSetEnvColor(state->dl++, 255, 255, 255, 196);
    }

    if (gem->flags & GEM_FLAGS_COLLECT_ANIM) {
        gDPSetRenderMode(state->dl++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    }
    
    Mtx* nextTransfrom = graphicsStateNextMtx(state);
    transformToMatrixL(data->transform, 1.0f / 256.0f, nextTransfrom);
    gSPMatrix(state->dl++, OS_K0_TO_PHYSICAL(nextTransfrom), G_MTX_MODELVIEW|G_MTX_MUL|G_MTX_PUSH);
    gSPDisplayList(state->dl++, _gem_mesh_tri_0);
    gSPPopMatrix(state->dl++, G_MTX_MODELVIEW);

    if (gem->flags & GEM_FLAGS_COLLECT_ANIM) {
        gDPSetRenderMode(state->dl++, G_RM_ZB_XLU_SURF, G_RM_ZB_XLU_SURF2);
    }
}

void gemUpdate(void* data) {
    struct Gem* gem = (struct Gem*)data;
    struct Quaternion combined;
    quatMultiply(&gem->transform.rotation, &gGemRotationPerFrame, &combined);
    gem->transform.rotation = combined;
}

void gemTrigger(void* data) {
    struct Gem* gem = (struct Gem*)data;

    struct CollisionBox bb;
    vector3Add(&gGemCollider.box.min, &gem->transform.position, &bb.min);
    vector3Add(&gGemCollider.box.max, &gem->transform.position, &bb.max);
    sparseCollisionReindex(&gSparseCollisionGrid, &gem->collider, 0, &bb);

    gem->flags |= GEM_FLAGS_COLLECT_ANIM | GEM_FLAGS_COLLECTED;
}

void gemInit(struct Gem* gem, struct Vector3* pos, short didCollect, short index) {
    transformIdentity(&gem->transform);
    gem->transform.position = *pos;

    gem->collider.collider= &gGemCollider;
    gem->collider.transform= &gem->transform;
    gem->collider.data = gem;
    gem->collider.trigger = gemTrigger;
    gem->collider.triggerMask = CollisionLayersMutualSwitch;

    timeAddListener(&gem->updateListener, gemUpdate, gem);

    struct CollisionBox bb;
    vector3Add(&gGemCollider.box.min, pos, &bb.min);
    vector3Add(&gGemCollider.box.max, pos, &bb.max);
    sparseCollisionReindex(&gSparseCollisionGrid, &gem->collider, &bb, 0);

    dynamicActorAddToGroup(&gScene.transparentActors, &gem->transform, gem, gemRender, TransparentMaterialTypeGem);
}