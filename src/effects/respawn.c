
#include "respawn.h"
#include "src/effects/explosion.h"

void respawnRender(struct DynamicActor* data, struct GraphicsState* state) {
    struct RespawnEffect* effect = (struct RespawnEffect*)data->data;
    Mtx* transform = graphicsStateNextMtx(state);

    float alphaLerp = 1.0f - teleportEffectGetQuickLerp(effect->teleportEffect);

    struct BasicTransform finalTransform = *data->transform;
    finalTransform.position.y = data->transform->position.y + 0.1f;

    transformToMatrixL(&finalTransform, effect->scale * alphaLerp / 256.0f, transform);

    u8 alpha = 255 - (u8)(255.0f * alphaLerp);
    gDPSetEnvColor(state->dl++, 128, 255, 255, alpha);

    gSPMatrix(state->dl++, OS_K0_TO_PHYSICAL(transform), G_MTX_MODELVIEW|G_MTX_MUL|G_MTX_PUSH);
    gSPDisplayList(state->dl++, _shockwave_mesh_tri_0);
    gDPPipeSync(state->dl++);
    gSPPopMatrix(state->dl++, G_MTX_MODELVIEW);
}

void respawnEffectStart(struct RespawnEffect* effect, struct BasicTransform* transform, float scale, struct TeleportEffect* teleport) {
    effect->teleportEffect = teleport;
    effect->scale = scale;
    effect->renderId = dynamicActorAddToGroup(&gScene.transparentActors, transform, effect, respawnRender, TransparentMaterialTypeShockwave, 1.0f);
}

void respawnEffectEnd(struct RespawnEffect* effect) {
    dynamicActorRemoveFromGroup(&gScene.transparentActors, &effect->renderId);
}