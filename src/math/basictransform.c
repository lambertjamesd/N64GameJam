
#include "basictransform.h"

void transformConcat(struct BasicTransform* a, struct BasicTransform* b, struct BasicTransform* result) {
    result->scale = a->scale * b->scale;
    struct Vector3 rotatedOffset;
    quatMultVector(&a->rotation, &b->position, &rotatedOffset);
    quatMultiply(&a->rotation, &b->rotation, &result->rotation);

    result->position.x = a->position.x + rotatedOffset.x * a->scale;
    result->position.y = a->position.y + rotatedOffset.y * a->scale;
    result->position.z = a->position.z + rotatedOffset.z * a->scale;
}

void transformPoint(struct BasicTransform* trans, struct Vector3* in, struct Vector3* out) {
    quatMultVector(&trans->rotation, in, out);
    vector3Scale(out, out, trans->scale);
    out->x += trans->position.x;
    out->y += trans->position.y;
    out->z += trans->position.z;
}

void transformInvert(struct BasicTransform* in, struct BasicTransform* out) {
    out->scale = 1.0f / in->scale;
    quatConjugate(&in->rotation, &out->rotation);
    quatMultVector(&out->rotation, &in->position, &out->position);
    out->position.x *= out->scale;
    out->position.y *= out->scale;
    out->position.z *= out->scale;
}