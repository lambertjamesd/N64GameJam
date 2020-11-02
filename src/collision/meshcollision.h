
#ifndef _COLLISION_MESHCOLLISION
#define _COLLISION_MESHCOLLISION

#include "collisiondata.h"

void collisionFaceBaryCoord(struct CollisionFace* face, struct Vector3* in, struct Vector3* baryCoord);
int collisionFaceCollideSphere(struct CollisionFace* face, struct Vector3* center, float radius, struct ContactPoint* contact);
int collisionEdgeCollideSphere(struct CollisionEdge* edge, struct Vector3* center, float radius, struct ContactPoint* contact);
int collisionPointCollideSphere(struct Vector3* point, struct Vector3* center, float radius, struct ContactPoint* contact);

int collisionMeshCollideSphere(struct CollisionMesh* mesh, struct Vector3* center, float radius, struct CollisionResult* result);

#endif