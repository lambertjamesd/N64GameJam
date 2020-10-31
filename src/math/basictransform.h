
#ifndef _MATH_BASICTRANSFORM_H
#define _MATH_BASICTRANSFORM_H

#include "quaternion.h"
#include "vector.h"

struct BasicTransform {
    struct Vector3 position;
    struct Quaternion rotation;
    float scale;
};

void transformConcat(struct BasicTransform* a, struct BasicTransform* b, struct BasicTransform* result);
void transformPoint(struct BasicTransform* trans, struct Vector3* in, struct Vector3* out);
void transformInvert(struct BasicTransform* in, struct BasicTransform* out);

#endif