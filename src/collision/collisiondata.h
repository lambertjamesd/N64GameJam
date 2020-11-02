
#ifndef _COLLISION_COLLISION_H
#define _COLLISION_COLLISION_H

#include "src/math/basictransform.h"
#include "src/math/plane.h"
#include "src/math/vector.h"

#define MAX_CONTACT_POINTS  16

enum CollisionLayers {
    CollisionLayersCadet = 1 << 0,
    CollisionLayersKillPlane = 1 << 1,
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
    void* data;
    TriggerCallback trigger;
};

int collisionColliderCollideSphere(struct CollisionCollider* collider, struct Vector3* center, float radius, struct CollisionResult* result);
int collisionTransColliderCollideSphere(struct CollisionTransformedCollider* collider, struct Vector3* center, float radius, struct CollisionResult* result);

#endif