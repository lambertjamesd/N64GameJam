
#ifndef _COLLISION_COLLISION_H
#define _COLLISION_COLLISION_H

#include "src/math/vector.h"

struct CollisionFace {
    struct Vector3 origin;
    float barycentricDenom;
    struct Vector3 dotCompare1;
    struct Vector3 dotCompare2;
};

void collisionFaceBaryCoord(struct CollisionFace* face, struct Vector3* in, struct Vector3* baryCoord);

#endif