
#ifndef _QUATERNION_H
#define _QUATERNION_H

#include "vector.h"

struct Quaternion {
    float x, y, z, w;
};

void quatIdent(struct Quaternion* q);
void quatAxisAngle(struct Vector3* axis, float angle, struct Quaternion* out);
void quatConjugate(struct Quaternion* in, struct Quaternion* out);
void quatMultVector(struct Quaternion* q, struct Vector3* a, struct Vector3* out);
void quatMultiply(struct Quaternion* a, struct Quaternion* b, struct Quaternion* out);
void quatToMatrix(struct Quaternion* q, float out[4][4]);

#endif