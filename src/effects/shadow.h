
#ifndef _GRAPHICS_SHADOW_H
#define _GRAPHICS_SHADOW_H

#include <ultra64.h>
#include "src/math/vector.h"
#include "src/graphics/dynamic.h"
#include "src/math/basictransform.h"

extern Gfx _drop_shadow_material[];
extern Gfx _shockwave_mesh_tri_0[];

struct DropShadowParams {
    float minSize;
    float maxSize;
    float minTrans;
    float maxTrans;
    int collisionMask;
};

struct DropShadow {
    struct Vector3 shadowPosition;
    struct Vector3 shadowUp;
    struct DropShadowParams* params;
    float shadowScale;
};

void dropShadowInit(struct DropShadow* shadow, struct BasicTransform* transform, struct DropShadowParams* params);
void dropShadowRender(struct DynamicActor* data, struct GraphicsState* state);
void dropShadowCalculate(struct DropShadow* shadow, int isGrounded, struct Vector3* from);

#endif