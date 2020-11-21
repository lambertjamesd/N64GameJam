
#ifndef EFFECTS_WALKANIM_H
#define EFFECTS_WALKANIM_H

#include "src/math/basictransform.h"

struct WalkAnim {
    float animationTime;
    float cycleDuration;
    float lastSpeed;
    float maxSpeed;
    float jumpHeight;
    float swayAngle;
};

void walkAnimInit(struct WalkAnim* walkAnim, float cycleDuration, float maxSpeed, float jumpHeight, float swayAngle);
void walkAnimUpdate(struct WalkAnim* walkAnim, float speed);
void walkAnimRelativeTransform(struct WalkAnim* walkAnum, struct BasicTransform* transform);

#endif