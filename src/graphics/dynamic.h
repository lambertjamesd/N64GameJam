
#ifndef _GRAPHICS_DYNAMIC_H
#define _GRAPHICS_DYNAMIC_H

#include "src/math/basictransform.h"

// Must be a power of 2
#define MAX_DYNAMIC_ACTORS 64
#define MAX_MATERIAL_GROUPS 8
#define MATERIAL_INDEX_NOT_BATCHED 8

enum DynamicMaterialType {
    DynamicMaterialTypeSwitch,
    DynamicMaterialTypeDoor,
    DynamicMaterialTypeBreakable,
};

struct GraphicsState {
    Gfx* dl;
    Mtx* matrices;
    int usedMatrices;
    int matrixCount;
    unsigned int primColor;
};

struct DynamicActor;

typedef void (*RenderCallback)(struct DynamicActor* data, struct GraphicsState* state);

struct DynamicActor {
    struct DynamicActor* next;
    struct BasicTransform* transform;
    void* data;
    RenderCallback render;
    int materialIndex;
};

struct DynamicActorGroup {
    struct DynamicActor actors[MAX_DYNAMIC_ACTORS];
    int nextActorId;
    // last element in the array is reserved for non batched types
    struct DynamicActor* actorByMaterial[MAX_MATERIAL_GROUPS + 1];
};

typedef int ActorId;

#define ACTOR_ID_NONE -1

Mtx* graphicsStateNextMtx(struct GraphicsState* state);
void graphicsStateSetPrimitiveColor(struct GraphicsState* state, u32 color);

ActorId dynamicActorAddToGroup(struct DynamicActorGroup* group, struct BasicTransform* transform, void* data, RenderCallback render, int materialIndex);
void dynamicActorRemoveFromGroup(struct DynamicActorGroup* group, ActorId* actorId);

void dynamicActorGroupRender(struct DynamicActorGroup* group, struct GraphicsState* state, Gfx** materials, int materialCount);
void dynamicActorGroupReset(struct DynamicActorGroup* group);

#endif