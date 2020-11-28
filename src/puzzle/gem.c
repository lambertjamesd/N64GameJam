
#include <math.h>

#include "gem.h"
#include "geo/gem.inc.c"
#include "src/collision/collisionscene.h"
#include "src/graphics/renderscene.h"
#include "src/level/level.h"
#include "src/save/savefile.h"
#include "src/effects/teleport.h"

struct DropShadowParams gGemShadowParams = {
    0.15,
    0.5f,
    84.0f,
    150.0f,
    CollisionLayersGeometry | CollisionLayersRobot | CollisionLayersSwamp,
};

struct CollisionCollider gGemCollider = {
    ColliderTypeBox,
    0,
    .box = {
        {-0.1466f, 0.0f, -0.1466f},
        {0.1466f, 0.4f, 0.1466f},
    }
};

struct Quaternion gGemRotationPerFrame = {
    0,
    0.017452406,
    0,
    0.999847695,
};

#define GEM_COLLECTED_ANIMATION_DURATION    1.5f
#define GEM_RAISE_TIME                      0.25f
#define GEM_FALL_TIME                       1.25f

#define GEM_RAISE_HEIGHT                    1.5f

#define GEM_END_ANIM_DURATION               2.0f
#define GEM_ORBIT_RADIUS                    0.8f
#define GEM_RADIUS_TIME                     0.5f
#define GEM_RAD_PER_SEC                     (M_PI * 1.8f)
#define GEM_TIME_OFFSET                     ((M_PI * 2.0f) / (GEM_RAD_PER_SEC * 3.0f))

void gemRender(struct DynamicActor* data, struct GraphicsState* state) {
    struct Gem* gem = (struct Gem*)data->data;

    if (gem->flags & GEM_FLAGS_COLLECT_ANIM) {
        gDPSetRenderMode(state->dl++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    }
    
    if (!(gem->flags & GEM_FLAGS_COLLECTED)) {
        if (gem->flags & GEM_FLAGS_PREVIOUSLY_COLLECTED) {
            gDPSetEnvColor(state->dl++, 0, 0, 0, 98);
        } else {
            gDPSetEnvColor(state->dl++, 255, 255, 255, 196);
        }

        
        Mtx* nextTransfrom = graphicsStateNextMtx(state);
        transformToMatrixL(data->transform, 1.0f / 256.0f, nextTransfrom);
        gSPMatrix(state->dl++, OS_K0_TO_PHYSICAL(nextTransfrom), G_MTX_MODELVIEW|G_MTX_MUL|G_MTX_PUSH);
        gSPDisplayList(state->dl++, _gem_mesh_tri_0);
        gDPPipeSync(state->dl++);
        gSPPopMatrix(state->dl++, G_MTX_MODELVIEW);

        if (gem->flags & GEM_FLAGS_COLLECT_ANIM) {
            gDPSetRenderMode(state->dl++, G_RM_ZB_XLU_SURF, G_RM_ZB_XLU_SURF2);
        }
    }
}


void gemUpdate(void* data) {
    struct Gem* gem = (struct Gem*)data;

    if (!(gem->flags & GEM_FLAGS_COLLECTED)) {
        struct Vector3 raycastOrigin;
        raycastOrigin.x = gem->transform.position.x;
        raycastOrigin.y = gem->transform.position.y + 0.1f;
        raycastOrigin.z = gem->transform.position.z;

        dropShadowCalculate(&gem->shadow, 0, &gem->transform.position);
    } else {
        dropShadowCalculate(&gem->shadow, 0, 0);
    }

    if (gem->flags & GEM_FLAGS_COLLECT_ANIM) {
        if (gem->animationTimer < GEM_RAISE_TIME) {
            float accelTime = (GEM_RAISE_TIME - gem->animationTimer);
            accelTime = 0.5f * accelTime * accelTime;

            gem->transform.position.y += accelTime * (GEM_RAISE_HEIGHT / GEM_RAISE_TIME);
        }

        if (gem->animationTimer > GEM_FALL_TIME) {
            float relativeTime = (GEM_COLLECTED_ANIMATION_DURATION - gem->animationTimer) *
                (1.0f / (GEM_COLLECTED_ANIMATION_DURATION - GEM_FALL_TIME));

            gem->transform.position.y += (1.0f - relativeTime * relativeTime) * 0.25f;

            gem->transform.scale = relativeTime;
        }

        gem->animationTimer += gTimeDelta;

        if (gem->animationTimer >= GEM_COLLECTED_ANIMATION_DURATION) {
            gem->flags &= ~GEM_FLAGS_COLLECT_ANIM;
            gem->flags |= GEM_FLAGS_COLLECTED;
        }
    } else if (gem->flags & GEM_FLAGS_END_ANIM) {
        float timeOffset = gem->animationTimer - gem->index * GEM_TIME_OFFSET;
        float radius = GEM_ORBIT_RADIUS;

        if (timeOffset < 0.0f) {
            timeOffset = 0.0f;
        }

        float angle = timeOffset * GEM_RAD_PER_SEC;

        if (timeOffset < GEM_RADIUS_TIME) {
            gem->transform.scale = timeOffset / GEM_RADIUS_TIME;
        }

        if (timeOffset > GEM_END_ANIM_DURATION) {
            gem->flags &= ~GEM_FLAGS_END_ANIM;
            gem->flags |= GEM_FLAGS_COLLECTED;
        } else if (timeOffset > GEM_END_ANIM_DURATION - GEM_RADIUS_TIME) {
            gem->transform.scale = (GEM_END_ANIM_DURATION - timeOffset) / GEM_RADIUS_TIME;
        }

        radius = gem->transform.scale * GEM_ORBIT_RADIUS;

        gem->transform.position.x = gem->cadetPos->x + radius * cosf(angle);
        gem->transform.position.y = gem->cadetPos->y + teleportEffectGetHeight(timeOffset) + 0.2f;
        gem->transform.position.z = gem->cadetPos->z + radius * sinf(angle);

        quatAxisAngle(&gUp, angle * 1.0f, &gem->transform.rotation);

        gem->animationTimer += gTimeDelta;
    } else {
        struct Quaternion combined;
        quatMultiply(&gem->transform.rotation, &gGemRotationPerFrame, &combined);
        gem->transform.rotation = combined;
    }
}

void gemTrigger(void* data, struct Vector3* origin) {
    struct Gem* gem = (struct Gem*)data;

    struct CollisionBox bb;
    vector3Add(&gGemCollider.box.min, &gem->transform.position, &bb.min);
    vector3Add(&gGemCollider.box.max, &gem->transform.position, &bb.max);
    sparseCollisionReindex(&gSparseCollisionGrid, &gem->collider, 0, &bb);
    saveFileMarkCollectedGem(gCurrentLevel, gem->index);
    struct Vector3 collectPos;
    collectPos.x = gem->transform.position.x;
    collectPos.y = gem->transform.position.y + 0.5f;
    collectPos.z = gem->transform.position.z;
    explosionInit(&gem->explosion, &gem->transform.position, ExplosionTypeGemCollect);

    gem->flags |= GEM_FLAGS_COLLECT_ANIM;
    gem->animationTimer = 0.0f;
}

void gemInit(struct Gem* gem, struct Vector3* pos, short index) {
    transformIdentity(&gem->transform);
    gem->transform.position = *pos;

    dropShadowInit(&gem->shadow, &gem->transform, &gGemShadowParams);

    gem->collider.collider= &gGemCollider;
    gem->collider.transform= &gem->transform;
    gem->collider.data = gem;
    gem->collider.trigger = gemTrigger;
    gem->collider.triggerMask = CollisionLayersMutualSwitch;

    gem->index = index;
    gem->flags = 0;

    timeAddListener(&gem->updateListener, gemUpdate, gem, TimeUpdateGroupWorld);
    explosionReset(&gem->explosion);

    struct CollisionBox bb;
    vector3Add(&gGemCollider.box.min, pos, &bb.min);
    vector3Add(&gGemCollider.box.max, pos, &bb.max);
    sparseCollisionReindex(&gSparseCollisionGrid, &gem->collider, &bb, 0);

    dynamicActorAddToGroup(&gScene.transparentActors, &gem->transform, gem, gemRender, TransparentMaterialTypeGem, 0.5f);

    if (saveFileDidCollectGem(gCurrentLevel, index)) {
        gem->flags |= GEM_FLAGS_PREVIOUSLY_COLLECTED;
    }
}

void gemEndCutscene(struct Gem* gem, struct Vector3* cadetPos) {
    if (gem->flags & GEM_FLAGS_COLLECT_ANIM) {
        gem->flags &= ~GEM_FLAGS_COLLECT_ANIM;
        gem->flags |= GEM_FLAGS_COLLECTED;
    }

    if (gem->flags & (GEM_FLAGS_COLLECTED | GEM_FLAGS_PREVIOUSLY_COLLECTED)) {
        gem->flags = GEM_FLAGS_END_ANIM;
        gem->animationTimer = 0.0f;
        gem->transform.position = *cadetPos;
        gem->transform.scale = 1.0f;
        gem->cadetPos = cadetPos;
        quatIdent(&gem->transform.rotation);
    }
}