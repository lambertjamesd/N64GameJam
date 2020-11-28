
#include "rockfragments.h"

#include "src/graphics/renderscene.h"
#include "src/level/level.h"
#include "src/math/mathf.h"

#define ROCK_FRAGMENT_LIFE          1.5f
#define ROCK_FRAGMENT_SCALE_START   1.0f
#define ROCK_FRAGMENT_GRAVITY       -9.8f
#define HEIGHT_OFFSET               1.0f
#define EXPLOSION_STRENGTH          7.5f
#define ANGULAR_VELOCITY            3.0f

void rockFragmentsRender(struct DynamicActor* data, struct GraphicsState* state) {
    struct RockFragments* fragments = (struct RockFragments*)data->data;

    int i;
    for (i = 0; i < fragments->fragmentCount; ++i) {
        Mtx* transform = graphicsStateNextMtx(state);
        transformToMatrixL(&fragments->transforms[i], 1.0f / 256.0f, transform);
        gSPMatrix(state->dl++, OS_K0_TO_PHYSICAL(transform), G_MTX_MODELVIEW|G_MTX_MUL|G_MTX_PUSH);
        gSPDisplayList(state->dl++, gLoadedLevel->theme->theme->themeMeshes[LevelThemeMeshesBreakFragment]);
        gSPPopMatrix(state->dl++, G_MTX_MODELVIEW);
    }
}

void rockFragmentUpdate(void* data) {
    struct RockFragments* fragments = (struct RockFragments*)data;

    int i;

    float scale;
    
    if (fragments->timer < ROCK_FRAGMENT_SCALE_START) {
        scale = 1.0f;
    } else {
        scale = (ROCK_FRAGMENT_LIFE - fragments->timer) / (ROCK_FRAGMENT_LIFE - ROCK_FRAGMENT_SCALE_START);
    }

    for (i = 0; i < fragments->fragmentCount; ++i) {
        fragments->velocity[i].y += gTimeDelta * ROCK_FRAGMENT_GRAVITY;
        vector3AddScaled(
            &fragments->transforms[i].position,
            &fragments->velocity[i],
            gTimeDelta,
            &fragments->transforms[i].position
        );
        struct Quaternion nextRotation;
        quatMultiply(
            &fragments->transforms[i].rotation,
            &fragments->angularVelocity[i],
            &nextRotation
        );
        fragments->transforms[i].rotation = nextRotation;
        fragments->transforms[i].scale = scale;
    }


    fragments->timer += gTimeDelta;

    if (fragments->timer >= ROCK_FRAGMENT_LIFE) {
        timeRemoveListener(&fragments->updateListener, TimeUpdateGroupWorld);
        dynamicActorRemoveFromGroup(&gScene.dynamicActors, &fragments->renderActorId);
    }
}

void rockFragmentsInit(struct RockFragments* fragments, struct Vector3* stackPos, int fragmentCount, struct Vector3* explosionOrigin) {
    if (fragmentCount > MAX_FRAGMENT_COUNT) {
        fragmentCount = MAX_FRAGMENT_COUNT;
    }

    fragments->fragmentCount = fragmentCount;

    float currY = HEIGHT_OFFSET;

    int i;
    for (i = 0; i < fragments->fragmentCount; ++i) {
        fragments->transforms[i].position.x = stackPos->x + mathfRandomFloat() - 0.5f;
        fragments->transforms[i].position.y = currY;
        fragments->transforms[i].position.z = stackPos->z + mathfRandomFloat() - 0.5f;
        quatRandom(&fragments->transforms[i].rotation);
        fragments->transforms[i].scale = 1.0f;

        struct Vector3 offset;
        vector3Sub(&fragments->transforms[i].position, explosionOrigin, &offset);

        float distance = vector3MagSqrd(&offset);

        if (distance == 0.0f) {
            fragments->velocity[i].x = 0.0f;
            fragments->velocity[i].y = 1.0f;
            fragments->velocity[i].z = 0.0f;
        } else {
            vector3Scale(&offset, &fragments->velocity[i], EXPLOSION_STRENGTH / distance);
        }

        quatMultVector(&fragments->transforms[i].rotation, &gUp, &offset);
        quatAxisAngle(&offset, (ANGULAR_VELOCITY * MIN_DELTA_TIME) / distance, &fragments->angularVelocity[i]);

        fragments->velocity[i].x = 0.0f;

        currY += HEIGHT_OFFSET;
    }

    fragments->timer = 0.0f;

    timeAddListener(&fragments->updateListener, rockFragmentUpdate, fragments, TimeUpdateGroupWorld);
    fragments->renderActorId = dynamicActorAddToGroup(&gScene.dynamicActors, &fragments->transforms[0], fragments, rockFragmentsRender, DynamicMaterialTypeBreakable, 2.0f);
}