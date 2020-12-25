
#include "splash.h"
#include "src/effects/geo/splash.inc.c"
#include "src/graphics/graphics.h"
#include "src/level/level.h"

#define INITIAL_VEL         11.0f
#define SPLASH_ACCEL        -20.8f
#define SPLASH_SCALE_RATE   9.0f
#define SPLASH_TIME         (-INITIAL_VEL/SPLASH_ACCEL)

#define MIN_SIZE            0.75f

void splashEffectRender(struct DynamicActor* data, struct GraphicsState* state) {
    if (!gLoadedLevel) {
        return;
    }

    struct SplashEffect* splash = (struct SplashEffect*)data->data;

    gDPPipeSync(state->dl++);
    gSPDisplayList(state->dl++, gLoadedLevel->theme->theme->materials[LevelThemeMaterialTypeLava]);
    gDPSetEnvColor(state->dl++, 210, 210, 210, 255);

    Mtx* baseMatrix = graphicsStateNextMtx(state);
    Mtx* splashMatrix = graphicsStateNextMtx(state);

    transformToMatrixL(data->transform, 1.0f / 256.0f, baseMatrix);

    struct BasicTransform relative;
    struct BasicTransform splashTransform;

    float sizeTime = splash->animationTime * splash->sizeScale;

    relative.position.x = 0.0f;
    relative.position.y = INITIAL_VEL*sizeTime + SPLASH_ACCEL*splash->animationTime*splash->animationTime;
    relative.position.z = 0.0f;
    quatIdent(&relative.rotation);
    relative.scale = 1.0f + sizeTime * SPLASH_SCALE_RATE;

    transformConcat(data->transform, &relative, &splashTransform);

    transformToMatrixL(&splashTransform, 1.0f / 256.0f, splashMatrix);

    gSPMatrix(state->dl++, OS_K0_TO_PHYSICAL(baseMatrix), G_MTX_MODELVIEW|G_MTX_MUL|G_MTX_PUSH);
    gSPVertex(state->dl++, Splash_mesh_vtx_0 + 0, 8, 0);
    gSPPopMatrix(state->dl++, G_MTX_MODELVIEW);

    gSPMatrix(state->dl++, OS_K0_TO_PHYSICAL(splashMatrix), G_MTX_MODELVIEW|G_MTX_MUL|G_MTX_PUSH);
    gSPDisplayList(state->dl++, OS_K0_TO_PHYSICAL(Splash_mesh_tri_1));
    gSPPopMatrix(state->dl++, G_MTX_MODELVIEW);
    gDPPipeSync(state->dl++);
}

void splashEffectUpdate(void* data) {
    struct SplashEffect* splash = (struct SplashEffect*)data;

    splash->animationTime += gTimeDelta;

    if (splash->animationTime > SPLASH_TIME*splash->sizeScale) {
        timeRemoveListener(&splash->updateListener, TimeUpdateGroupWorld);
        dynamicActorRemoveFromGroup(&gScene.dynamicActors, &splash->renderId);
    }
}

void splashEffectInit(struct SplashEffect* splash, struct Vector3* at, float sizeScale) {
    splash->animationTime = 0.0f;
    splash->transform.position = *at;

    if (sizeScale < MIN_SIZE) {
        splash->sizeScale = MIN_SIZE;
    } else {
        splash->sizeScale = sizeScale;
    }
    
    quatIdent(&splash->transform.rotation);
    splash->transform.scale = 1.0f;

    if (!timeHasListener(&splash->updateListener, TimeUpdateGroupWorld)) {
        timeAddListener(&splash->updateListener, splashEffectUpdate, splash, TimeUpdateGroupWorld);
        splash->renderId = dynamicActorAddToGroup(&gScene.dynamicActors, &splash->transform, splash, splashEffectRender, MATERIAL_INDEX_NOT_BATCHED, 1.0f);
    }
}

int splashEffectActive(struct SplashEffect* splash) {
    return splash->animationTime <= SPLASH_TIME*splash->sizeScale && timeHasListener(&splash->updateListener, TimeUpdateGroupWorld);
}