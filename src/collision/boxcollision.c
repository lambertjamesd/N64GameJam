
#include "boxcollision.h"

#include "meshcollision.h"
#include "src/math/ray.h"
#include "src/math/mathf.h"
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

float collisionBoxRaycast(struct CollisionBox* mesh, struct Vector3* origin, struct Vector3* dir, struct ContactPoint* contact) {
    int xContained = contact->point.x + NEAR_ZERO >= mesh->min.x && contact->point.x - NEAR_ZERO <= mesh->max.x;
    int yContained = contact->point.y + NEAR_ZERO >= mesh->min.y && contact->point.y - NEAR_ZERO <= mesh->max.y;
    int zContained = contact->point.z + NEAR_ZERO >= mesh->min.z && contact->point.z - NEAR_ZERO <= mesh->max.z;

    if (dir->x < -NEAR_ZERO || dir->x > NEAR_ZERO) {
        float distaceCheck = ((dir->x > 0.0f ? mesh->min.x : mesh->max.x) - origin->x) / dir->x;

        if (distaceCheck >= 0.0f) {
            rayPointAtDistance(origin, dir, distaceCheck, &contact->point);

            if (yContained && zContained) {
                contact->normal.x = -fsign(dir->x);
                contact->normal.y = 0.0f;
                contact->normal.z = 0.0f;

                return distaceCheck;
            }
        }
    }

    if (dir->y < -NEAR_ZERO || dir->y > NEAR_ZERO) {
        float distaceCheck = ((dir->y > 0.0f ? mesh->min.y : mesh->max.y) - origin->y) / dir->y;

        if (distaceCheck >= 0.0f) {
            rayPointAtDistance(origin, dir, distaceCheck, &contact->point);

            if (xContained && zContained) {
                contact->normal.x = 0.0f;
                contact->normal.y = -fsign(dir->y);
                contact->normal.z = 0.0f;

                return distaceCheck;
            }
        }
    }

    if (dir->z < -NEAR_ZERO || dir->z > NEAR_ZERO) {
        float distaceCheck = ((dir->z > 0.0f ? mesh->min.z : mesh->max.z) - origin->z) / dir->z;

        if (distaceCheck >= 0.0f) {
            rayPointAtDistance(origin, dir, distaceCheck, &contact->point);

            if (xContained && yContained) {
                contact->normal.x = 0.0f;
                contact->normal.y = 0.0f;
                contact->normal.z = -fsign(dir->z);

                return distaceCheck;
            }
        }
    }

    return RAYCAST_NO_HIT;
}