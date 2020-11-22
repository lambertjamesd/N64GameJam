
#include "explosion.h"

#include "geo/explosion.inc.c"
#include "src/collision/collisionscene.h"
#include "src/graphics/graphics.h"

#define EXPLOSION_DURATION      1.0f
#define EXPLOSION_RADIUS        1.0f

void explosionRender(struct DynamicActor* data, struct GraphicsState* state) {
    struct Explosion* explosion = (struct Explosion*)data;

    Mtx* transform = graphicsStateNextMtx(state);
    transformToMatrixL(data->transform, 1.0f / 256.0f, transform);
    gSPMatrix(state->dl++, OS_K0_TO_PHYSICAL(transform), G_MTX_MODELVIEW|G_MTX_MUL|G_MTX_PUSH);
	gDPSetEnvColor(state->dl++, 0, 0, 0, 255 - (u8)(explosion->timer / EXPLOSION_DURATION));
    gSPDisplayList(state->dl++, _shockwave_mesh_tri_0);
    gDPPipeSync(state->dl++);
    gSPPopMatrix(state->dl++, G_MTX_MODELVIEW);
}

void explosionCleanup(struct Explosion* explosion) {
    dynamicActorRemoveFromGroup(&gScene.transparentActors, &explosion->renderId);
    timeRemoveListener(&explosion->updateListener, TimeUpdateGroupWorld);
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

void explosionInit(struct Explosion* explosion, struct Vector3* position) {
    explosion->transform.position = *position;
    quatIdent(&explosion->transform.rotation);
    explosion->transform.scale = 1.0f;
    explosion->timer = 0.0f;

    timeAddListener(&explosion->updateListener, explosionUpdate, explosion, TimeUpdateGroupWorld);

    explosion->renderId = dynamicActorAddToGroup(&gScene.transparentActors, &explosion->transform, explosion, explosionRender, TransparentMaterialTypeShockwave);
}