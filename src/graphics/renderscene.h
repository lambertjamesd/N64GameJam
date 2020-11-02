
#ifndef _GRAPHICS_RENDERSCENE_H
#define _GRAPHICS_RENDERSCENE_H

#include "src/math/basictransform.h"
#include "src/time/time.h"
#include "levelgraphics.h"
#include "levelthemegraphics.h"

#define START_FOLLOW_DISTANCE 10.0f
#define CAMERA_MAX_SPEED 10.0f

struct SceneCamera {
    struct BasicTransform transform;
    struct Vector3 centerPosition;
    float followDistance;
    struct TimeUpdateListener updateListener;
    struct Vector3 targetPosition;
    float targetFollowDistance;
};

void cameraInit(struct SceneCamera* camera);
void cameraCleanup(struct SceneCamera* camera);

struct RenderScene {
    struct SceneCamera camera;
    struct LevelTileGrid* levelGrid;
    struct LevelThemeGraphics* theme;
};

extern struct RenderScene gScene;

#endif