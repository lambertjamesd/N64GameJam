
#include "rockettrail.h"
#include "explosion.h"

#include "src/graphics/graphics.h"
#include "src/math/mathf.h"

#define STOP_ANIMATING  100.0f

void rocketTrailRender(struct DynamicActor* data, struct GraphicsState* state) {
    struct RocketTrail* trail = (struct RocketTrail*)data->data;

    gDPPipeSync(state->dl++);

    int i;

    for (i = 0; i < trail->particleCount; ++i) {
        struct BasicTransform* currentTransform = &trail->particleTransforms[i];

        if (currentTransform->scale == 0.0f) {
            continue;
        }

        Mtx* transform = graphicsStateNextMtx(state);
        if (trail->flags & ROCKET_TRAIL_FLAGS_RELATIVE) {
            struct BasicTransform combined;
            transformConcat(data->transform, currentTransform, &combined);
            transformToMatrixL(&combined, 1.0f / 256.0f, transform);
        } else {
            transformToMatrixL(currentTransform, 1.0f / 256.0f, transform);
        }

        float lerp = (currentTransform->scale - trail->parameters->minScale) / 
            (trail->parameters->maxScale - trail->parameters->minScale);

        u8 alpha = (u8)mathfLerp((u8)trail->alpha, 0.0f, lerp);

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

    for (i = 0; i < trail->particleCount; ++i) {
        struct BasicTransform* currentTransform = &trail->particleTransforms[i];
        if (currentTransform->scale != 0.0f) {
            currentTransform->scale += gTimeDelta * trail->parameters->scaleRate;

            if (currentTransform->scale >= trail->parameters->maxScale) {
                currentTransform->scale = 0.0f;
            } else {
                struct Vector3 velocity;
                quatMultVector(&currentTransform->rotation, &gUp, &velocity);
                vector3AddScaled(&currentTransform->position, &velocity, trail->parameters->moveSpeed * gTimeDelta, &currentTransform->position);
                hasActive = 1;
            }
        } else if (nextParticle == -1) {
            nextParticle = i;
        }
    }

    if (trail->spawnTimer <= 0.0f && nextParticle != -1) {
        struct BasicTransform* currentTransform = &trail->particleTransforms[nextParticle];

        if (trail->flags & ROCKET_TRAIL_FLAGS_RELATIVE) {
            quatIdent(&currentTransform->rotation);
            currentTransform->position = gZeroVec;
            currentTransform->scale = trail->parameters->minScale;
        } else {
            currentTransform->rotation = trail->emitSource->rotation;
            transformPoint(trail->emitSource, &trail->parameters->emitOffset, &currentTransform->position);
            currentTransform->scale = trail->parameters->minScale;
        }

        trail->spawnTimer = trail->parameters->spawnInterval;
    } else if (trail->spawnTimer == STOP_ANIMATING) {
        if (!hasActive) {
            timeRemoveListener(&trail->updateListener, TimeUpdateGroupWorld);
            dynamicActorRemoveFromGroup(&gScene.transparentActors, &trail->renderId);
        }
    } else {
        trail->spawnTimer -= gTimeDelta;
    }
}

void rocketTrailStart(struct RocketTrail* trail, struct BasicTransform *emitSource, struct RocketTrailParameters* parameters, short particleCount, char flags) {
    int i;

    for (i = 0; i < particleCount; ++i) {
        trail->particleTransforms[i] = *emitSource;
        trail->particleTransforms[i].scale = 0.0f;
    }

    trail->emitSource = emitSource;
    trail->parameters = parameters;
    trail->spawnTimer = 0.0f; 
    trail->alpha = 255;
    trail->flags = flags;
    trail->particleCount = particleCount;

    if (!timeHasListener(&trail->updateListener, TimeUpdateGroupWorld)) {
        timeAddListener(&trail->updateListener, rocketTrailUpdate, trail, TimeUpdateGroupWorld);
        trail->renderId = dynamicActorAddToGroup(&gScene.transparentActors, emitSource, trail, rocketTrailRender, parameters->material, 20.0f);
    }    
}

void rocketTrailStop(struct RocketTrail* trail) {
    trail->spawnTimer = STOP_ANIMATING;
}