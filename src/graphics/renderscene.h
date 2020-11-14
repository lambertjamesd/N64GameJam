
#ifndef _GRAPHICS_RENDERSCENE_H
#define _GRAPHICS_RENDERSCENE_H

#include "src/math/basictransform.h"
#include "src/math/vector2.h"
#include "src/time/time.h"
#include "levelgraphics.h"
#include "levelthemegraphics.h"
#include "dynamic.h"

#define START_FOLLOW_DISTANCE 10.0f
#define CAMERA_MAX_SPEED 20.0f

struct SceneCamera {
    struct BasicTransform transform;
    struct Vector3 centerPosition;
    float followDistance;
    struct TimeUpdateListener updateListener;
    struct Vector3 targetPosition;
    float currentRotation;
    float targetRotation;
    float currentPitch;
    short followDistanceStep;
    short targetDistanceStep;
};

void cameraInit(struct SceneCamera* camera);
void cameraCleanup(struct SceneCamera* camera);
void cameraGetMoveDir(struct SceneCamera* camera, struct Vector2* input, struct Vector2* out);

struct RenderScene {
    struct SceneCamera camera;
    struct DynamicActorGroup dynamicActors;
    struct DynamicActorGroup transparentActors;
    Gfx* transparentMaterials[MAX_MATERIAL_GROUPS];
    Gfx* transparentMaterialCleanup[MAX_MATERIAL_GROUPS];
    struct LevelTileGrid* levelGrid;
    struct LevelThemeGraphics* theme;
};

extern struct RenderScene gScene;

void renderSceneReset(struct RenderScene* scene);

#endif