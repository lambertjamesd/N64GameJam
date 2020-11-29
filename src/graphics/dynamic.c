
#include "dynamic.h"

LookAt* graphicsStateNextLookat(struct GraphicsState* state) {
    if (state->usedLookAt < state->lookAtCount) {
        LookAt* result = &state->lookats[state->usedLookAt];
        ++state->usedLookAt;
        return result;
    }

    return 0;
}

Mtx* graphicsStateNextMtx(struct GraphicsState* state) {
    if (state->usedMatrices < state->matrixCount) {
        Mtx* result = &state->matrices[state->usedMatrices];
        ++state->usedMatrices;
        return result;
    }

    return 0;
}

void graphicsStateSetPrimitiveColor(struct GraphicsState* state, u32 color) {
    gDPSetPrimColor(state->dl++, 0, 0, (color & 0xFF000000) >> 24, (color & 0xFF0000) >> 16, (color & 0xFF00) >> 8, color & 0xFF);
}

int dynamicActorAddToGroup(struct DynamicActorGroup* group, struct BasicTransform* transform, void* data, RenderCallback render, int materialIndex, float radius) {
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
    actor->radius = radius;

    return result;
}

void dynamicActorRemoveFromGroup(struct DynamicActorGroup* group, ActorId* actorId) {
    if (*actorId == ACTOR_ID_NONE) {
        return;
    }

    struct DynamicActor* actor = &group->actors[*actorId];

    if (actor->render) {
        struct DynamicActor* prev = 0;
        struct DynamicActor* curr = group->actorByMaterial[actor->materialIndex];

        while (curr) {
            if (curr == actor) {
                break;
            }

            prev = curr;
            curr = curr->next;
        }

        if (curr) {
            if (prev) {
                prev->next = curr->next;
            } else {
                group->actorByMaterial[actor->materialIndex] = curr->next;
            }
            curr->next = 0;
        }

        actor->render = 0;
    }

    *actorId = ACTOR_ID_NONE;
}

void dynamicActorRenderChain(struct DynamicActor* actor, struct GraphicsState* state, Gfx* mat, Gfx* matCleanup) {
    int usedMaterial = 0;

    while (actor) {
        int isVisible = !planeIsSphereBehind(
            &state->frustumPlanes[0], 
            &actor->transform->position,
            actor->radius
        ) && !planeIsSphereBehind(
            &state->frustumPlanes[1], 
            &actor->transform->position,
            actor->radius
        ) && !planeIsSphereBehind(
            &state->frustumPlanes[2], 
            &actor->transform->position,
            actor->radius
        ) && !planeIsSphereBehind(
            &state->frustumPlanes[3], 
            &actor->transform->position,
            actor->radius
        );

        if (isVisible) {
            if (!usedMaterial && mat) {
                gSPDisplayList(state->dl++, mat);
                usedMaterial = 1;
            }
            actor->render(actor, state);
        }

        actor = actor->next;
    }

    if (usedMaterial && matCleanup) {
        gSPDisplayList(state->dl++, matCleanup);
    }
}

void dynamicActorGroupRender(struct DynamicActorGroup* group, struct GraphicsState* state, Gfx** materials, Gfx** materialCleanup, int materialCount) {
    int i;

    for (i = 0; i < materialCount && i < MAX_MATERIAL_GROUPS; ++i) {
        dynamicActorRenderChain(group->actorByMaterial[i], state, materials[i], materialCleanup[i]);
    }

    dynamicActorRenderChain(group->actorByMaterial[MATERIAL_INDEX_NOT_BATCHED], state, 0, 0);
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