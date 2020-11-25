
#ifndef _ROCKET_ROCKET_H
#define _ROCKET_ROCKET_H

#include "src/math/basictransform.h"
#include "src/graphics/dynamic.h"

struct Rocket {
    struct BasicTransform transform;
};

extern struct Rocket gRocket;

void rocketRender(struct DynamicActor* data, struct GraphicsState* state);

#endif