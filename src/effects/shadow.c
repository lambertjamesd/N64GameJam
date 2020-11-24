
#include "shadow.h"
#include "geo/dropshadow.inc.c"
#include "src/collision/collisionscene.h"
#include "src/graphics/graphics.h"
#include "src/math/mathf.h"
#include "src/input/controller.h"

#define MAX_SHADOW_DISTANCE 4.0f
#define SHADOW_HIDE -10000.0f

void matrixSitAt(struct Vector3* at, struct Vector3* forward, struct Vector3* up, float scale, float mtx[4][4]) {
    struct Vector3 side;
    vector3Cross(forward, up, &side);
    vector3Normalize(&side, &side);
    struct Vector3 modifedForward;
    vector3Cross(up, &side, &modifedForward);

    mtx[0][0] = side.x * scale;
    mtx[0][1] = side.y * scale;
    mtx[0][2] = side.z * scale;
    mtx[0][3] = 0.0f;
    
    mtx[1][0] = up->x * scale;
    mtx[1][1] = up->y * scale;
    mtx[1][2] = up->z * scale;
    mtx[1][3] = 0.0f;
    
    mtx[2][0] = modifedForward.x * scale;
    mtx[2][1] = modifedForward.y * scale;
    mtx[2][2] = modifedForward.z * scale;
    mtx[2][3] = 0.0f;

    mtx[3][0] = at->x;
    mtx[3][1] = at->y;
    mtx[3][2] = at->z;
    mtx[3][3] = 1.0f;
}

void dropShadowInit(struct DropShadow* shadow, struct BasicTransform* transform, struct DropShadowParams* params) {
    shadow->shadowPosition.x = 0.0f;
    shadow->shadowPosition.y = SHADOW_HIDE;
    shadow->shadowPosition.z = 0.0f;
    shadow->shadowUp = gUp;
    shadow->params = params;
    shadow->shadowScale = 1.0f;
    dynamicActorAddToGroup(&gScene.transparentActors, transform, shadow, dropShadowRender, TransparentMaterialTypeShadow);
}

void dropShadowRender(struct DynamicActor* data, struct GraphicsState* state) {
    struct DropShadow* shadow = (struct DropShadow*)data->data;
    struct DropShadowParams* params = shadow->params;

    if (shadow->shadowPosition.y == SHADOW_HIDE) {
        return;
    }

    Mtx* shadowTransform = graphicsStateNextMtx(state);
    float mtx[4][4];
    matrixSitAt(
        &shadow->shadowPosition, 
        &gForward, 
        &shadow->shadowUp, 
        mathfLerp(params->minSize, params->maxSize, shadow->shadowScale) / 256.0f, 
        mtx
    );

    guMtxF2L(mtx, shadowTransform);
    gSPMatrix(state->dl++, OS_K0_TO_PHYSICAL(shadowTransform), G_MTX_MODELVIEW|G_MTX_MUL|G_MTX_PUSH);
	gDPSetEnvColor(state->dl++, 0, 0, 0, (u8)mathfLerp(params->minTrans, params->maxTrans, shadow->shadowScale));
    gSPDisplayList(state->dl++, _quad_mesh_tri_0);
    gDPPipeSync(state->dl++);
    gSPPopMatrix(state->dl++, G_MTX_MODELVIEW);
}

void dropShadowCalculate(struct DropShadow* shadow, int isGrounded, struct Vector3* from) {
    if (isGrounded) {
        shadow->shadowPosition = *from;
        shadow->shadowUp = gUp;
        shadow->shadowScale = 1.0f;
        shadow->shadowPosition.y += 0.1f;
    } else if (from) {
        struct ContactPoint hit;
        struct Vector3 down = {0.0f, -1.0f, 0.0f};

        float result = collisionSceneRaycast(from, &down, shadow->params->collisionMask, MAX_SHADOW_DISTANCE, &hit);

        if (result == RAYCAST_NO_HIT) {
            shadow->shadowPosition.y = SHADOW_HIDE;
        } else {
            shadow->shadowPosition = hit.point;
            shadow->shadowUp = hit.normal;
            shadow->shadowPosition.y += 0.1f;
            
            if (result > MAX_SHADOW_DISTANCE) {
                shadow->shadowScale = 0.0f;
            } else {
                shadow->shadowScale = 1.0f - result * (1.0f / MAX_SHADOW_DISTANCE); 
            }
        }
    } else {
        shadow->shadowPosition.y = SHADOW_HIDE;
    }
} 