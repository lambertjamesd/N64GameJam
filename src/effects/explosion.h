
#ifndef EFFECTS_EXPLOSION_H
#define EFFECTS_EXPLOSION_H

#include <ultra64.h>
#include "src/math/basictransform.h"
#include "src/time/time.h"

extern Gfx _shockwave_mat[];
extern Gfx _shockwave_mesh_tri_0[];
extern Gfx _explosion_spike_mesh_tri_0[];

enum ExplosionType {
    ExplosionTypeRobot,
    ExplosionTypeGemCollect,
};

struct Explosion {
    struct BasicTransform transform;
    struct TimeUpdateListener updateListener;
    float timer;
    int renderId;
    enum ExplosionType type;
};

void explosionReset(struct Explosion* explosion);
void explosionInit(struct Explosion* explosion, struct Vector3* position, enum ExplosionType type);
int explosionIsActive(struct Explosion* explosion);

#endif