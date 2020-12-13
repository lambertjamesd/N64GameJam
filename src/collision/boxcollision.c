
#include "boxcollision.h"

#include "meshcollision.h"
#include "collisiondata.h"
#include "src/math/ray.h"
#include "src/math/mathf.h"
#include <math.h>

int collisionBoxCollideSphere(struct Box* box, struct Vector3* center, float radius, struct CollisionResult* result) {
    struct ContactPoint* contact = &result->contacts[result->contactCount];
    float wideRadius = radius + COLLIDE_EXTRA_RADIUS;

    if (box->min.x >= center->x + wideRadius || box->max.x <= center->x - wideRadius ||
        box->min.y >= center->y + wideRadius || box->max.y <= center->y - wideRadius ||
        box->min.z >= center->z + wideRadius || box->max.z <= center->z - wideRadius
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
        if (collisionPointCollideSphere(&contact->point, center, wideRadius, contact)) {
            vector3AddScaled(&contact->point, &contact->normal, radius, center);
            ++result->contactCount;
            return 1;
        }
    }

    return 0;
}

int collisionBoxOverlapSphere(struct Box* box, struct Vector3* center, float radius) {
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

float collisionBoxRaycast(struct Box* mesh, struct Vector3* origin, struct Vector3* dir, struct ContactPoint* contact) {
    int xContained = origin->x + NEAR_ZERO >= mesh->min.x && origin->x - NEAR_ZERO <= mesh->max.x;
    int yContained = origin->y + NEAR_ZERO >= mesh->min.y && origin->y - NEAR_ZERO <= mesh->max.y;
    int zContained = origin->z + NEAR_ZERO >= mesh->min.z && origin->z - NEAR_ZERO <= mesh->max.z;

    if (dir->x < -NEAR_ZERO || dir->x > NEAR_ZERO) {
        float distaceCheck = ((dir->x > 0.0f ? mesh->min.x : mesh->max.x) - origin->x) / dir->x;

        if (distaceCheck >= 0.0f && yContained && zContained) {
            rayPointAtDistance(origin, dir, distaceCheck, &contact->point);
            contact->normal.x = -fsign(dir->x);
            contact->normal.y = 0.0f;
            contact->normal.z = 0.0f;

            return distaceCheck;
        }
    }

    if (dir->y < -NEAR_ZERO || dir->y > NEAR_ZERO) {
        float distaceCheck = ((dir->y > 0.0f ? mesh->min.y : mesh->max.y) - origin->y) / dir->y;

        if (distaceCheck >= 0.0f && xContained && zContained) {
            rayPointAtDistance(origin, dir, distaceCheck, &contact->point);
            contact->normal.x = 0.0f;
            contact->normal.y = -fsign(dir->y);
            contact->normal.z = 0.0f;

            return distaceCheck;
        }
    }

    if (dir->z < -NEAR_ZERO || dir->z > NEAR_ZERO) {
        float distaceCheck = ((dir->z > 0.0f ? mesh->min.z : mesh->max.z) - origin->z) / dir->z;

        if (distaceCheck >= 0.0f && xContained && yContained) {
            rayPointAtDistance(origin, dir, distaceCheck, &contact->point);
            contact->normal.x = 0.0f;
            contact->normal.y = 0.0f;
            contact->normal.z = -fsign(dir->z);

            return distaceCheck;
        }
    }

    return RAYCAST_NO_HIT;
}