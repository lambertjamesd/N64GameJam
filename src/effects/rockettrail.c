
#include "rockettrail.h"
#include "explosion.h"

#include "src/graphics/graphics.h"

#define STOP_ANIMATING  100.0f

#define MOVE_SPEED 2.0f
#define SCALE_RATE 0.5f
#define MAX_SCALE 1.0f
#define MIN_SCALE 0.1f

#define SPAWN_INTERVAL 0.5f

void rocektTrailRender(struct DynamicActor* data, struct GraphicsState* state) {
    struct RocketTrail* trail = (struct RocketTrail*)data->data;

    gDPPipeSync(state->dl++);

    int i;

    for (i = 0; i < MAX_ACTIVE_PARTICLES; ++i) {
        struct BasicTransform* currentTransform = &trail->particleTransforms[i];

        if (currentTransform->scale == 0.0f) {
            continue;
        }

        Mtx* transform = graphicsStateNextMtx(state);
        transformToMatrixL(currentTransform, 1.0f / 256.0f, transform);

        u8 alpha = trail->alpha - (u8)((float)trail->alpha * currentTransform->scale / MAX_SCALE);

        gDPSetEnvColor(state->dl++, 255, 255, 255, alpha);
        gSPMatrix(state->dl++, OS_K0_TO_PHYSICAL(transform), G_MTX_MODELVIEW|G_MTX_MUL|G_MTX_PUSH);
        gSPDisplayList(state->dl++, _shockwave_mesh_tri_0);
        gDPPipeSync(state->dl++);
        gSPPopMatrix(state->dl++, G_MTX_MODELVIEW);
    }
}

void rocketTrailUpdate(void* data) {
    struct RocketTrail* trail = (struct RocketTrail*)data;

    int nextParticle = -1;
    int hasActive = 0;

    int i;

    for (i = 0; i < MAX_ACTIVE_PARTICLES; ++i) {
        struct BasicTransform* currentTransform = &trail->particleTransforms[i];
        if (currentTransform->scale != 0.0f) {
            currentTransform->scale += gTimeDelta * SCALE_RATE;

            if (currentTransform->scale >= MAX_SCALE) {
                currentTransform->scale = 0.0f;
            } else {
                struct Vector3 velocity;
                quatMultVector(&currentTransform->rotation, &gUp, &velocity);
                vector3AddScaled(&currentTransform->position, &velocity, -MOVE_SPEED * gTimeDelta, &currentTransform->position);
                hasActive = 1;
            }
        } else if (nextParticle == -1) {
            nextParticle = i;
        }
    }

    if (trail->spawnTimer <= 0.0f && nextParticle != -1) {
        struct BasicTransform* currentTransform = &trail->particleTransforms[nextParticle];

        currentTransform->rotation = trail->emitSource->rotation;
        transformPoint(trail->emitSource, &trail->emitOffset, &currentTransform->position);
        currentTransform->scale = MIN_SCALE;

        trail->spawnTimer = SPAWN_INTERVAL;
    } else if (trail->spawnTimer == STOP_ANIMATING) {
        if (!hasActive) {
            timeRemoveListener(&trail->updateListener, TimeUpdateGroupWorld);
            dynamicActorRemoveFromGroup(&gScene.transparentActors, &trail->renderId);
        }
    } else {
        trail->spawnTimer -= gTimeDelta;
    }
}

void rocektTrailStart(struct RocketTrail* trail, struct BasicTransform *emitSource, struct Vector3* emitOffset) {
    int i;

    for (i = 0; i < MAX_ACTIVE_PARTICLES; ++i) {
        trail->particleTransforms[i] = *emitSource;
        trail->particleTransforms[i].scale = 0.0f;
    }

    trail->emitSource = emitSource;
    trail->emitOffset = *emitOffset;
    trail->spawnTimer = 0.0f; 
    trail->alpha = 255;

    timeAddListener(&trail->updateListener, rocketTrailUpdate, trail, TimeUpdateGroupWorld);
    trail->renderId = dynamicActorAddToGroup(&gScene.transparentActors, emitSource, trail, rocektTrailRender, TransparentMaterialTypeShockwave, 20.0f);
}

void rocektTrailStop(struct RocketTrail* trail) {
    trail->spawnTimer = STOP_ANIMATING;
}