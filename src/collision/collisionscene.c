
#include "collisionscene.h"
#include "src/system/memory.h"

struct CollisionResult* collisionSceneCollideSphere(struct Vector3* position, float radius, int collisionMask) {
    struct CollisionResult* result = fastMalloc(sizeof(struct CollisionResult), ALIGNMENT_OF(struct CollisionResult));
}