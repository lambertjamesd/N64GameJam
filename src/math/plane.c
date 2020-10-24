
#include "plane.h"

void planeFromNormalPoint(struct Vector3* normal, struct Vector3* point, struct Plane* out) {
    out->a = normal->x;
    out->b = normal->y;
    out->c = normal->z;
    out->d = -(normal->x * point->x + normal->y * point->y + normal->z * point->z);
}

void planeProjectOnto(struct Plane* plane, struct Vector3* point, struct Vector3* out) {
    float distance = planeDistanceFromPoint(plane, point);
    out->x = point->x - plane->a * distance;
    out->y = point->y - plane->b * distance;
    out->z = point->z - plane->c * distance;
}

float planeDistanceFromPoint(struct Plane* plane, struct Vector3* point) {
    return plane->a * point->x + plane->b * point->y + plane->c * point->z + plane->d;
}