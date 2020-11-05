
#include "boxcollision.h"

#include "meshcollision.h"
#include <math.h>

int collisionBoxCollideSphere(struct CollisionBox* box, struct Vector3* center, float radius, struct CollisionResult* result) {
    struct ContactPoint* contact = &result->contacts[result->contactCount];

    if (box->min.x >= center->x + radius || box->max.x <= center->x - radius ||
        box->min.y >= center->y + radius || box->max.y <= center->y - radius ||
        box->min.z >= center->z + radius || box->max.z <= center->z - radius
    ) {
        return 0;
    }

    int i;
    int containedCount = 0;

    for (i = 0; i < 3; ++i) {
        if (center->el[i] < box->min.el[i]) {
            contact->point.el[i] = box->min.el[i];
        } else if (center->el[i] > box->max.el[i]) {
            contact->point.el[i] = box->max.el[i];
        } else {
            contact->point.el[i] = center->el[i];
            ++containedCount;
        }
    }

    if (containedCount == 3) {
        float edgeDifference = MAXFLOAT;
        int edgeIndex = 0;

        for (i = 0; i < 3; ++i) {
            contact->point.el[i] = center->el[i] < (box->min.el[i] + box->max.el[i]) * 0.5f ? box->min.el[i] : box->max.el[i];
            float faceDist = fabsf(contact->point.el[i] - center->el[i]);

            if (faceDist < edgeDifference) {
                edgeDifference = faceDist;
                edgeIndex = i;
            }
        }

        for (i = 0; i < 3; ++i) {
            if (i != edgeIndex) {
                contact->point.el[i] = center->el[i];
                contact->normal.el[i] = 0.0f;
            } else {
                if (center->el[i] < (box->min.el[i] + box->max.el[i]) * 0.5f) {
                    contact->point.el[i] = box->min.el[i];
                    contact->normal.el[i] = -1.0f;
                } else {
                    contact->point.el[i] = box->max.el[i];
                    contact->normal.el[i] = 1.0f;
                }
            }
        }

        contact->collisionMask = 0;
        contact->transform = 0;

        vector3AddScaled(&contact->point, &contact->normal, radius, center);
        ++result->contactCount;

        return 1;
    } else if (containedCount == 2) {
        for (i = 0; i < 3; ++i) {
            if (contact->point.el[i] == center->el[i]) {
                contact->normal.el[i] = 0.0f;
            } else {
                if (center->el[i] < box->min.el[i]) {
                    contact->normal.el[i] = -1.0f;
                } else {
                    contact->normal.el[i] = 1.0f;
                }
            }
        }

        contact->collisionMask = 0;
        contact->transform = 0;

        vector3AddScaled(&contact->point, &contact->normal, radius, center);
        ++result->contactCount;

        return 1;
    } else {
        if (collisionPointCollideSphere(&contact->point, center, radius, contact)) {
            vector3AddScaled(&contact->point, &contact->normal, radius, center);
            ++result->contactCount;
            return 1;
        }
    }

    return 0;
}

int collisionBoxOverlapSphere(struct CollisionBox* box, struct Vector3* center, float radius) {
    struct Vector3 nearestPoint;
    int i;
    for (i = 0; i < 3; ++i) {
        if (center->el[i] < box->min.el[i]) {
            nearestPoint.el[i] = box->min.el[i];
        } else if (center->el[i] > box->max.el[i]) {
            nearestPoint.el[i] = box->max.el[i];
        } else {
            nearestPoint.el[i] = center->el[i];
        }
    }

    return vector3DistSqrd(&nearestPoint, center) < radius * radius;
}