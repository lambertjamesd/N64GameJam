
#ifndef _MATH_BASICTRANSFORM_H
#define _MATH_BASICTRANSFORM_H

#include <ultra64.h>

#include "quaternion.h"
#include "vector.h"

struct BasicTransform {
    struct Vector3 position;
    struct Quaternion rotation;
    float scale;
};

void transformConcat(struct BasicTransform* a, struct BasicTransform* b, struct BasicTransform* result);
void transformPoint(struct BasicTransform* trans, struct Vector3* in, struct Vector3* out);
void transformPointInverse(struct BasicTransform* trans, struct Vector3* in, struct Vector3* out);
void transformInvert(struct BasicTransform* in, struct BasicTransform* out);
void transformToMatrix(struct BasicTransform* in, float worldScale, float mtx[4][4]);
void transformToMatrixL(struct BasicTransform* in, float worldScale, Mtx* mtx);

void transformIdentity(struct BasicTransform* trans);

#endif