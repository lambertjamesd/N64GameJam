
#include "explosion.h"

#include "geo/explosion.inc.c"
#include "src/collision/collisionscene.h"
#include "src/graphics/graphics.h"

#define EXPLOSION_DURATION          0.25f
#define EXPLOSION_RADIUS            1.0f
#define EXPLOSION_TIMER_INACTIVE    -1.0f

void explosionRender(struct DynamicActor* data, struct GraphicsState* state) {
    struct Explosion* explosion = (struct Explosion*)data->data;

    Mtx* transform = graphicsStateNextMtx(state);
    transformToMatrixL(data->transform, 1.0f / 256.0f, transform);

    Mtx* spikeTransform;
    
    if (explosion->type == ExplosionTypeRobot) {
        spikeTransform = graphicsStateNextMtx(state);

        float mtx[4][4];

        float actualScale = data->transform->scale;
        data->transform->scale = 1.0f;

        transformToMatrix(data->transform, 1.0f / 256.0f, mtx);

        float horzScale = 1.0f - actualScale;

        mtx[0][0] *= horzScale;
        mtx[0][1] *= horzScale;
        mtx[0][2] *= horzScale;

        mtx[1][0] *= actualScale;
        mtx[1][1] *= actualScale;
        mtx[1][2] *= actualScale;

        mtx[2][0] *= horzScale;
        mtx[2][1] *= horzScale;
        mtx[2][2] *= horzScale;

        guMtxF2L(mtx, spikeTransform);
        data->transform->scale = actualScale;
    }

    gSPMatrix(state->dl++, OS_K0_TO_PHYSICAL(transform), G_MTX_MODELVIEW|G_MTX_MUL|G_MTX_PUSH);

    float alphaLerp = explosion->transform.scale * explosion->transform.scale;
    u8 alpha = 255 - (u8)(255.0f * alphaLerp);

    if (explosion->type == ExplosionTypeRobot) {
        gDPSetEnvColor(state->dl++, (alpha >> 2) + 127, alpha, 12, alpha);
    } else {
        gDPSetEnvColor(state->dl++, 10, alpha, (alpha >> 2) + 127, alpha);
    }
    gSPDisplayList(state->dl++, _shockwave_mesh_tri_0);
    gDPPipeSync(state->dl++);
    gSPPopMatrix(state->dl++, G_MTX_MODELVIEW);

    if (explosion->type == ExplosionTypeRobot) {
        gSPMatrix(state->dl++, OS_K0_TO_PHYSICAL(spikeTransform), G_MTX_MODELVIEW|G_MTX_MUL|G_MTX_PUSH);
        gDPSetCombineLERP(state->dl++, 0, 0, 0, ENVIRONMENT, 0, 0, 0, ENVIRONMENT, 0, 0, 0, ENVIRONMENT, 0, 0, 0, ENVIRONMENT);
        gSPDisplayList(state->dl++, _explosion_spike_mesh_tri_0);
        gDPPipeSync(state->dl++);
        gDPSetCombineLERP(state->dl++, TEXEL0, 0, ENVIRONMENT, 0, TEXEL0, 0, ENVIRONMENT, 0, TEXEL0, 0, ENVIRONMENT, 0, TEXEL0, 0, ENVIRONMENT, 0);
        gDPPipeSync(state->dl++);
        gSPPopMatrix(state->dl++, G_MTX_MODELVIEW);
    }
}

void explosionCleanup(struct Explosion* explosion) {
    dynamicActorRemoveFromGroup(&gScene.transparentActors, &explosion->renderId);
    timeRemoveListener(&explosion->updateListener, TimeUpdateGroupWorld);
    explosion->timer = EXPLOSION_TIMER_INACTIVE;
}

void explosionUpdate(void* data) {
    struct Explosion* explosion = (struct Explosion*)data;
    if (explosion->timer  >= EXPLOSION_DURATION) {
        explosionCleanup(explosion);
    } else {
        explosion->transform.scale = (EXPLOSION_RADIUS / EXPLOSION_DURATION) * explosion->timer;

        explosion->timer += gTimeDelta;
    }
}


void explosionReset(struct Explosion* explosion) {
    explosion->timer = EXPLOSION_TIMER_INACTIVE;
}

void explosionInit(struct Explosion* explosion, struct Vector3* position, enum ExplosionType type) {
    if (explosionIsActive(explosion)) {
        explosionCleanup(explosion);
    }

    explosion->transform.position = *position;
    quatIdent(&explosion->transform.rotation);
    explosion->transform.scale = 0.0f;
    explosion->timer = 0.0f;
    explosion->type = type;

    timeAddListener(&explosion->updateListener, explosionUpdate, explosion, TimeUpdateGroupWorld);

    explosion->renderId = dynamicActorAddToGroup(&gScene.transparentActors, &explosion->transform, explosion, explosionRender, TransparentMaterialTypeShockwave);
}


int explosionIsActive(struct Explosion* explosion) {
    return explosion->timer != EXPLOSION_TIMER_INACTIVE;
}