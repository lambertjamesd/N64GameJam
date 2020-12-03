
#include "meshcollision.h"
#include "src/math/ray.h"

#define COLLIDE_EXTRA_RADIUS        0.01f

void collisionFaceBaryCoord(struct CollisionFace* face, struct Vector3* in, struct Vector3* baryCoord) {
    struct Vector3 relative;
    vector3Sub(in, &face->origin, &relative);
    baryCoord->y = vector3Dot(&face->dotCompare1, &relative);
    baryCoord->z = vector3Dot(&face->dotCompare2, &relative);
    baryCoord->x = 1.0f - baryCoord->y - baryCoord->z;
}

int collisionFaceIsBaryInside(struct CollisionFace* face, struct Vector3* baryCoord) {
    if (face->faceType == CollisionFaceTypeTriangle) {
        if (baryCoord->x < 0.0f || baryCoord->y < 0.0f || baryCoord->z < 0.0f) {
            return 0;
        }
    } else {
        if (baryCoord->y < 0.0f || baryCoord->z < 0.0f || baryCoord->y > 1.0f || baryCoord->z > 1.0f) {
            return 0;
        }
    }

    return 1;
}

int collisionFaceCollideSphere(struct CollisionFace* face, struct Vector3* center, float radius, struct ContactPoint* contact) {
    float faceDistance = planeDistanceFromPoint(&face->plane, center);

    if (faceDistance >= radius || faceDistance <= -radius) {
        return 0;
    }

    vector3AddScaled(center, &face->plane.normal, -faceDistance, &contact->point);

    struct Vector3 baryCoord;
    collisionFaceBaryCoord(face, &contact->point, &baryCoord);

    if (!collisionFaceIsBaryInside(face, &baryCoord)) {
        return 0;
    }

    contact->normal = face->plane.normal;
    contact->transform = 0;

    return 1;
}

int collisionEdgeCollideSphere(struct CollisionEdge* edge, struct Vector3* center, float radius, struct ContactPoint* contact) {
    struct Vector3 relative;
    vector3Sub(center, &edge->origin, &relative);
    float edgeLerp = vector3Dot(&relative, &edge->dir);

    if (edgeLerp < 0.0f || edgeLerp > edge->length) {
        return 0;
    }

    vector3AddScaled(&edge->origin, &edge->dir, edgeLerp, &contact->point);

    struct Vector3 overlap;
    vector3Sub(center, &contact->point, &overlap);

    float overlapLen = vector3MagSqrd(&overlap);

    if (overlapLen > radius * radius) {
        return 0;
    } else if (overlapLen < 0.00001f) {
        contact->normal = gRight;
    } else {
        vector3Scale(&overlap, &contact->normal, 1.0f / sqrtf(overlapLen));
    }

    contact->transform = 0;

    return 1;
}

int collisionPointCollideSphere(struct Vector3* point, struct Vector3* center, float radius, struct ContactPoint* contact) {
    struct Vector3 overlap;
    vector3Sub(center, point, &overlap);

    float overlapLen = vector3MagSqrd(&overlap);

    if (overlapLen > radius * radius) {
        return 0;
    } else if (overlapLen < 0.00001f) {
        contact->normal = gRight;
    } else {
        vector3Scale(&overlap, &contact->normal, 1.0f / sqrtf(overlapLen));
    }

    contact->point = *point;
    contact->transform = 0;

    return 1;
}

int collisionMeshCollideSphere(struct CollisionMesh* mesh, struct Vector3* center, float radius, struct CollisionResult* result) {
    int didCollide = 0;
    int i;

    float wideRadius = radius + COLLIDE_EXTRA_RADIUS;

    for (i = 0; i < mesh->faceCount; ++i) {
        struct ContactPoint* contact = &result->contacts[result->contactCount];
        if (collisionFaceCollideSphere(&mesh->faces[i], center, wideRadius, contact)) {
            vector3AddScaled(&contact->point, &contact->normal, radius, center);
            ++result->contactCount;
            didCollide = 1;

            if (result->contactCount == MAX_CONTACT_POINTS) {
                return 1;
            }
        }
    }

    if (didCollide) {
        return 1;
    }

    for (i = 0; i < mesh->edgeCount; ++i) {
        struct ContactPoint* contact = &result->contacts[result->contactCount];
        if (collisionEdgeCollideSphere(&mesh->edges[i], center, wideRadius, contact)) {
            vector3AddScaled(&contact->point, &contact->normal, radius, center);
            ++result->contactCount;
            didCollide = 1;

            if (result->contactCount == MAX_CONTACT_POINTS) {
                return 1;
            }
        }
    }

    if (didCollide) {
        return 1;
    }

    for (i = 0; i < mesh->pointCount; ++i) {
        struct ContactPoint* contact = &result->contacts[result->contactCount];
        if (collisionPointCollideSphere(&mesh->points[i].origin, center, wideRadius, contact)) {
            vector3AddScaled(&contact->point, &contact->normal, radius, center);
            ++result->contactCount;
            didCollide = 1;

            if (result->contactCount == MAX_CONTACT_POINTS) {
                return 1;
            }
        }
    }

    return didCollide;
}

float collisionPlaneRaycast(struct Plane* plane, struct Vector3* origin, struct Vector3* dir, struct ContactPoint* contact) {
    float dot = vector3Dot(&plane->normal, dir);

    if (dot > -NEAR_ZERO) {
        return RAYCAST_NO_HIT;
    }

    float result = -(vector3Dot(&plane->normal, origin) + plane->d) / dot;

    rayPointAtDistance(origin, dir, result, &contact->point);
    contact->normal = plane->normal;

    return result;
}

float collisionMeshRaycast(struct CollisionMesh* mesh, struct Vector3* origin, struct Vector3* dir, struct ContactPoint* contact) {
    float result = RAYCAST_NO_HIT;
    int i;

    for (i = 0; i < mesh->faceCount; ++i) {
        struct CollisionFace* face = &mesh->faces[i];
        struct ContactPoint contactCheck;

        float faceDistance = collisionPlaneRaycast(&face->plane, origin, dir, &contactCheck);

        if (faceDistance < result && faceDistance >= 0.0f) {
            struct Vector3 baryCoord;
            collisionFaceBaryCoord(face, &contactCheck.point, &baryCoord);

            if (collisionFaceIsBaryInside(face, &baryCoord)) {
                result = faceDistance;
                contact->point = contactCheck.point;
                contact->normal = contactCheck.normal;
            }
        }
    }

    return result;
}