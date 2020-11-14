
#include "entranceexit.h"
#include "src/collision/geo/geo.h"
#include "src/collision/collisionscene.h"
#include "src/graphics/renderscene.h"
#include "src/level/level.h"
#include "src/math/mathf.h"

void exitEntranceRender(struct DynamicActor* data, struct GraphicsState* state) {
    Gfx* toRender = gLoadedLevel->theme->theme->themeMeshes[LevelThemeMeshesEntranceExit];

    Mtx* nextTransfrom = graphicsStateNextMtx(state);

    transformToMatrixL(data->transform, 1.0f / 256.0f, nextTransfrom);
    gSPMatrix(state->dl++, OS_K0_TO_PHYSICAL(nextTransfrom), G_MTX_MODELVIEW|G_MTX_MUL|G_MTX_PUSH);
    gSPDisplayList(state->dl++, toRender);
    gSPPopMatrix(state->dl++, G_MTX_MODELVIEW);
}

void entranceExitUpdate(void* data) {

}

void entranceExitInit(struct EntranceExit* exit, struct Vector3* pos, int isCadet) {
    transformIdentity(&exit->transform);
    exit->transform.position = *pos;
    
    if (isCadet) {
        exit->transform.scale = 0.5f;
    } else {
        exit->transform.scale = 1.0f;
    }
    
    exit->collider.collider = &_entrance_exit_collider;
    exit->collider.transform = &exit->transform;
    exit->collider.data = 0;
    exit->collider.trigger = 0;
    exit->collider.triggerMask = 0;

    timeAddListener(&exit->updateListener, entranceExitUpdate, exit);

    struct CollisionBox bb;
    bb.min.x = pos->x - 1.0f;
    bb.min.y = pos->y;
    bb.min.z = pos->z - 1.0f;

    bb.max.x = pos->x + 1.0f;
    bb.max.y = pos->y + 0.15f;
    bb.max.z = pos->z + 1.0f;
    sparseCollisionReindex(&gSparseCollisionGrid, &exit->collider, &bb, 0);

    dynamicActorAddToGroup(&gScene.dynamicActors, &exit->transform, exit, exitEntranceRender, DynamicMaterialTypeEntranceExit);
}

struct EntranceExit gCadetExit;
struct EntranceExit gRobotExit;