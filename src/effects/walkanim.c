
#include "walkanim.h"
#include "src/time/time.h"
#include "src/math/mathf.h"


void walkAnimInit(struct WalkAnim* walkAnim, float cycleDuration, float maxSpeed, float jumpHeight, float swayAngle) {
    walkAnim->animationTime = 0.0f;
    walkAnim->cycleDuration = cycleDuration;
    walkAnim->lastSpeed = 0.0f;
    walkAnim->maxSpeed = maxSpeed;
    walkAnim->jumpHeight = jumpHeight;
    walkAnim->swayAngle = swayAngle;
}

void walkAnimUpdate(struct WalkAnim* walkAnim, float speed) {
    walkAnim->lastSpeed = mathfMoveTowards(walkAnim->lastSpeed,  speed / walkAnim->maxSpeed, 5.0f * gTimeDelta);
    walkAnim->animationTime += gTimeDelta * walkAnim->lastSpeed;

    if (walkAnim->animationTime >= walkAnim->cycleDuration) {
        walkAnim->animationTime -= walkAnim->cycleDuration;
        walkAnim->swayAngle = -walkAnim->swayAngle;
    }
}

void walkAnimRelativeTransform(struct WalkAnim* walkAnim, struct BasicTransform* transform) {
    float lerpValue = (2.0 * walkAnim->animationTime / walkAnim->cycleDuration) - 1.0f;

    float heightOffset = (1.0f - lerpValue * lerpValue) * walkAnim->jumpHeight * walkAnim->lastSpeed;
    transform->position.x = 0.0f;
    transform->position.y = heightOffset;
    transform->position.z = 0.0f;
    
    quatAxisAngle(&gForward, walkAnim->swayAngle * lerpValue * walkAnim->lastSpeed, &transform->rotation);
    transform->scale = 1.0f;
}