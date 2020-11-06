
#include "dynamic.h"

Mtx* graphicsStateNextMtx(struct GraphicsState* state) {
    if (state->usedMatrices < state->matrixCount) {
        Mtx* result = &state->matrices[state->usedMatrices];
        ++state->usedMatrices;
        return result;
    }

    return 0;
}

int dynamicActorAddToGroup(struct DynamicActorGroup* group, struct BasicTransform* transform, void* data, RenderCallback render, int materialIndex) {
    int result = group->nextActorId;

    while (group->actors[result].render) {
        result = (result + 1) & (MAX_DYNAMIC_ACTORS - 1);
        
        if (result == group->nextActorId) {
            // all actor slots are full
            return -1;
        }
    }

    struct DynamicActor* actor = &group->actors[result];

    actor->next = group->actorByMaterial[materialIndex];
    group->actorByMaterial[materialIndex] = actor;
    actor->transform = transform;
    actor->data = data;
    actor->render = render;
    actor->materialIndex = materialIndex;

    return result;
}

void dynamicActorRemoveFromGroup(struct DynamicActorGroup* group, int actorId) {

}

void dynamicActorRenderChain(struct DynamicActor* actor, struct GraphicsState* state) {
    while (actor) {
        actor->render(actor, state);
        actor = actor->next;
    }
}

void dynamicActorGroupRender(struct DynamicActorGroup* group, struct GraphicsState* state, Gfx** materials, int materialCount) {
    int i;

    for (i = 0; i < materialCount && i < MAX_MATERIAL_GROUPS; ++i) {
        if (group->actorByMaterial[i]) {
            if (materials[i]) {
                gSPDisplayList(state->dl, materials[i]);
            }

            dynamicActorRenderChain(group->actorByMaterial[i], state);
        }
    }

    dynamicActorRenderChain(group->actorByMaterial[MATERIAL_INDEX_NOT_BATCHED], state);
}

void dynamicActorGroupReset(struct DynamicActorGroup* group) {
    int i;
    for (i = 0; i < MAX_DYNAMIC_ACTORS; ++i) {
        group->actors[i].render = 0;
    }

    for (i = 0; i <= MATERIAL_INDEX_NOT_BATCHED; ++i) {
        group->actorByMaterial[i] = 0;
    }

    group->nextActorId = 0;
}