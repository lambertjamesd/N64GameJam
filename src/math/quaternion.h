
#ifndef _QUATERNION_H
#define _QUATERNION_H

#include "vector.h"
#include "vector2.h"

struct Quaternion {
    float x, y, z, w;
};

void quatIdent(struct Quaternion* q);
void quatAxisAngle(struct Vector3* axis, float angle, struct Quaternion* out);
void quatAxisComplex(struct Vector3* axis, struct Vector2* complex, struct Quaternion* out);
void quatConjugate(struct Quaternion* in, struct Quaternion* out);
void quatMultVector(struct Quaternion* q, struct Vector3* a, struct Vector3* out);
void quatMultiply(struct Quaternion* a, struct Quaternion* b, struct Quaternion* out);
void quatToMatrix(struct Quaternion* q, float out[4][4]);
void quatNormalize(struct Quaternion* q, struct Quaternion* out);
void quatRandom(struct Quaternion* q);
void quatEulerAngles(struct Vector3* angles, struct Quaternion* out);

#endif