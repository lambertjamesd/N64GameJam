
#include "movingplatform.h"

#include "src/collision/collisionscene.h"
#include "src/graphics/renderscene.h"
#include "src/level/level.h"

struct CollisionCollider gMovingPlatformCollider = {
    ColliderTypeBox,
    CollisionLayersGeometry,
    .box = {
        {-1.0f, -0.5f, -1.0f},
        {1.0f, 0.0f, 1.0f},
    }
};

struct MovingPlatformSlot* movingPlatformGetEmptySlotInDir(struct MovingPlatformSlot* platform, int dir) {
    struct MovingPlatformSlot* result = platform->adjacent[dir];

    if (result && !result->occupant) {
        return result;
    } else {
        return 0;
    }
}

int movingPlatformNextSlotDir(struct MovingPlatformSlot* platform, int dir) {
    struct MovingPlatformSlot* result = movingPlatformGetEmptySlotInDir(platform, dir);

    if (result) {
        return dir;
    }

    result = movingPlatformGetEmptySlotInDir(platform, (dir + 1) & 0x3);

    if (result) {
        return (dir + 1) & 0x3;
    }

    result = movingPlatformGetEmptySlotInDir(platform, (dir + 3) & 0x3);

    if (result) {
        return (dir + 3) & 0x3;
    }

    return -1;
}

struct MovingPlatformSlot* movingPlatformClockwiseSlot(struct MovingPlatformSlot* platform, int* dir) {
    int i;

    for (i = 0; i < 4; ++i) {
        struct MovingPlatformSlot* check = movingPlatformGetEmptySlotInDir(platform, *dir);

        if (check) {
            return check;
        }

        *dir = (*dir + 1) & 0x3;
    }

    return 0;
}

void movingPlatformRender(struct DynamicActor* data, struct GraphicsState* state) {
    struct MovingPlatform* platform = (struct MovingPlatform*)data->data;
    Gfx* toRender = gLoadedLevel->theme->theme->themeMeshes[LevelThemeMeshesMovingPlatform];

    Mtx* nextTransfrom = graphicsStateNextMtx(state);
    graphicsStateSetPrimitiveColor(state, gSwitchDarkColors[platform->signalIndex]);
    transformToMatrixL(data->transform, 1.0f / 256.0f, nextTransfrom);
    gSPMatrix(state->dl++, OS_K0_TO_PHYSICAL(nextTransfrom), G_MTX_MODELVIEW|G_MTX_MUL|G_MTX_PUSH);
    gSPDisplayList(state->dl++, toRender);
    gSPPopMatrix(state->dl++, G_MTX_MODELVIEW);
}

void movingPlatformUpdate(void* data) {
    struct MovingPlatform* platform = (struct MovingPlatform*)data;

    if (gCurrentSignal[platform->signalIndex] != platform->lastSignalValue) {
        if (platform->moveDirection != -1) {
            platform->currentSlot = platform->currentSlot->adjacent[platform->moveDirection];
            platform->moveDirection = (platform->moveDirection + 2) & 0x3;
        } else {
            int nextDir = platform->nextMoveDir;
            if (nextDir == -1) {
                nextDir = 0;
            }

            struct MovingPlatformSlot* next = movingPlatformClockwiseSlot(platform->currentSlot, &nextDir);

            if (next) {
                next->occupant = platform;
                platform->moveDirection = nextDir;
            } else {
                platform->nextMoveDir = (nextDir + 2) & 0x3;
            }
        }

        platform->lastSignalValue = gCurrentSignal[platform->signalIndex];
    }

    if (platform->moveDirection != -1) {
        struct MovingPlatformSlot* next = platform->currentSlot->adjacent[platform->moveDirection];

        if (next) {
            vector3MoveTowards(
                &platform->transform.position,
                &next->position,
                gTimeDelta * PLATFORM_MOVE_SPEED,
                &platform->transform.position
            );

            if (platform->transform.position.x == next->position.x &&
                platform->transform.position.y == next->position.y &&
                platform->transform.position.z == next->position.z) {
                
                platform->currentSlot->occupant = 0;
                platform->currentSlot = next;

                int nextDir = movingPlatformNextSlotDir(next, platform->moveDirection);

                if (nextDir == -1) {
                    platform->nextMoveDir = (platform->moveDirection + 2) & 0x3;
                    platform->moveDirection = -1;
                } else {
                    platform->moveDirection = nextDir;
                    next->adjacent[nextDir]->occupant = platform;
                }
            }
        } else {
            platform->transform.position = platform->currentSlot->position;
            platform->nextMoveDir = (platform->moveDirection + 2) & 0x3;
            platform->moveDirection = -1;
        }

        struct CollisionBox box;
        vector3Add(&gMovingPlatformCollider.box.min, &platform->transform.position, &box.min);
        vector3Add(&gMovingPlatformCollider.box.max, &platform->transform.position, &box.max);
        sparseCollisionReindex(&gSparseCollisionGrid, &platform->collider, &box, &platform->prevBB);
        platform->prevBB = box;
    }
}

void movingPlatformInit(struct MovingPlatform* platform, struct Vector3* position, struct MovingPlatformSlot* slot, int color) {
    transformIdentity(&platform->transform);
    platform->transform.position = *position;

    platform->collider.collider = &gMovingPlatformCollider;
    platform->collider.transform = &platform->transform;
    platform->collider.data = 0;
    platform->collider.trigger = 0;
    platform->collider.triggerMask = 0;

    timeAddListener(&platform->updateListener, movingPlatformUpdate, platform);

    platform->signalIndex = color;
    platform->lastSignalValue = 0;

    platform->currentSlot = slot;
    platform->moveDirection = -1;
    platform->nextMoveDir = -1;
    slot->occupant = platform;

    vector3Add(&gMovingPlatformCollider.box.min, position, &platform->prevBB.min);
    vector3Add(&gMovingPlatformCollider.box.max, position, &platform->prevBB.max);
    sparseCollisionReindex(&gSparseCollisionGrid, &platform->collider, &platform->prevBB, 0);

    dynamicActorAddToGroup(&gScene.dynamicActors, &platform->transform, platform, movingPlatformRender, DynamicMaterialTypeMovingPlatform);
}

void movingPlatformJoinSlots(struct MovingPlatformSlot* platform, int count) {
    int i, j;

    for (i = 0; i < count; ++i) {
        for (j = 0; j < i; ++j) {
            struct MovingPlatformSlot* a = &platform[i];
            struct MovingPlatformSlot* b = &platform[j];

            if (a->position.z == b->position.z) {
                if (a->position.x + 2.0f == b->position.x) {
                    a->adjacent[0] = b;
                    b->adjacent[2] = a;
                } else if (a->position.x - 2.0f == b->position.x) {
                    a->adjacent[2] = b;
                    b->adjacent[0] = a;
                }
            } else if (a->position.x == b->position.x) {
                if (a->position.z + 2.0f == b->position.z) {
                    a->adjacent[1] = b;
                    b->adjacent[3] = a;
                } else if (a->position.z - 2.0f == b->position.z) {
                    a->adjacent[3] = b;
                    b->adjacent[1] = a;
                }
            }
        }
    }
}

void movingPlatformSlotInit(struct MovingPlatformSlot* platform, struct Vector3* position) {
    platform->adjacent[0] = 0;
    platform->adjacent[1] = 0;
    platform->adjacent[2] = 0;
    platform->adjacent[3] = 0;

    platform->occupant = 0;
    platform->position = *position;
}