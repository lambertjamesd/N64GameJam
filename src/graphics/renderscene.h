
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
#define SPLIT_SCREEN_BORDER_SIZE 2

#define MAX_VIEWPORTS    2

struct SceneViewport {
    int minx, maxx, miny, maxy;
};

void viewportConvert(struct SceneViewport* input, Vp* output);

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
    int controllerIndex;
};

void cameraSetFollowDistance(struct SceneCamera* camera, int followDistance);
void cameraInit(struct SceneCamera* camera, struct Vector3* startTarget, int controllerIndex);
void cameraCleanup(struct SceneCamera* camera);
void cameraGetMoveDir(struct SceneCamera* camera, struct Vector2* input, struct Vector2* out);

struct RenderScene {
    struct SceneCamera camera[MAX_VIEWPORTS];
    struct SceneViewport viewports[MAX_VIEWPORTS];
    float fov[MAX_VIEWPORTS];
    int activeViewportCount;
    
    struct DynamicActorGroup dynamicActors;
    struct DynamicActorGroup transparentActors;
    Gfx* transparentMaterials[MAX_MATERIAL_GROUPS];
    Gfx* transparentMaterialCleanup[MAX_MATERIAL_GROUPS];
    struct LevelTileGrid* levelGrid;
    struct LevelThemeGraphics* theme;
};

extern struct RenderScene gScene;

void renderSceneReset(struct RenderScene* scene);
void renderSceneSetViewportSplit(int target);
void renderSceneSetTargetViewportSplit(int target);
void renderSceneUpdateSplit();
void renderSceneMoveViewportSplit(int target, int maxMove, int isVert);

#endif