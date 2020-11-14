
#ifndef _COLLISION_COLLISION_H
#define _COLLISION_COLLISION_H

#include "src/math/basictransform.h"
#include "src/math/plane.h"
#include "src/math/vector.h"

#define MAX_CONTACT_POINTS  16

#define RAYCAST_NO_HIT  MAXFLOAT

#define NEAR_ZERO   0.000001f

enum CollisionLayers {
    CollisionLayersGeometry = 1 << 0,
    CollisionLayersRobot = 1 << 1,
    CollisionLayersRobotSwitch = 1 << 2,
    CollisionLayersMutualSwitch = 1 << 3,
    CollisionLayersCadetSwitch = 1 << 4,
    CollisionLayersBreakable = 1 << 5,
    CollisionLayersSwamp = 1 << 6,
    CollisionLayersKillPlane = 1 << 7,
};

struct ContactPoint {
    struct Vector3 point;
    struct Vector3 normal;
    struct BasicTransform* transform;
    int collisionMask;
};

struct CollisionResult {
    struct ContactPoint* contacts;
    int contactCount;
};

enum CollisionFaceType {
    CollisionFaceTypeTriangle,
    CollisionFaceTypeRectangle,
};

struct CollisionFace {
    struct Plane plane;
    struct Vector3 origin;
    struct Vector3 dotCompare1;
    struct Vector3 dotCompare2;
    enum CollisionFaceType faceType;
};

struct CollisionEdge {
    struct Vector3 origin;
    struct Vector3 dir;
    float length;
};

struct CollisionPoint {
    struct Vector3 origin;
};

struct CollisionMesh {
    struct CollisionFace* faces;
    int faceCount;
    struct CollisionEdge* edges;
    int edgeCount;
    struct CollisionPoint* points;
    int pointCount;
};

struct CollisionBox {
    struct Vector3 min;
    struct Vector3 max;
};

enum ColliderType {
    ColliderTypeMesh,
    ColliderTypeBox,
};

struct CollisionCollider {
    enum ColliderType type;
    int collisionMask;
    union {
        struct CollisionBox box;
        struct CollisionMesh mesh;
    };
};

typedef void (*TriggerCallback)(void* data);

struct CollisionTransformedCollider {
    struct CollisionCollider* collider;
    struct BasicTransform* transform;
    int triggerMask;
    void* data;
    TriggerCallback trigger;
};

int collisionColliderCollideSphere(struct Vector3* center, float radius, struct CollisionCollider* collider, int collisionMask, struct CollisionResult* result);
int collisionColliderOverlapSphere(struct CollisionCollider* collider, struct Vector3* center, float radius);
int collisionTransColliderCollideSphere(struct Vector3* center, float radius, struct CollisionTransformedCollider* collider, int collisionMask, struct CollisionResult* result);

float collisionColliderRaycast(struct CollisionCollider* collider, struct Vector3* origin, struct Vector3* dir, int collisionMask, struct ContactPoint* contact);
float collisionTransColliderRaycast(struct CollisionTransformedCollider* collider, struct Vector3* origin, struct Vector3* dir, int collisionMask, struct ContactPoint* contact);

#endif