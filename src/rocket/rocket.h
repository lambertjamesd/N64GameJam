
#ifndef _ROCKET_ROCKET_H
#define _ROCKET_ROCKET_H

#include "src/math/basictransform.h"
#include "src/graphics/dynamic.h"
#include "src/graphics/color.h"

struct Rocket {
    struct BasicTransform transform;
    struct Color color;
};

extern struct Rocket gRocket;

void rocketRender(struct DynamicActor* data, struct GraphicsState* state);

#endif