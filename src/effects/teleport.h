
#ifndef _EFFECTS_TELEPORT_H
#define _EFFECTS_TELEPORT_H

#define TELEPORT_FLAG_ACTIVE 1
#define TELEPORT_FLAG_REVERSE 2
#define TELEPORT_FLAG_QUICK 4

#include <ultra64.h>
#include "src/math/basictransform.h"

struct TeleportEffect {
    float time;
    int flags;
};

void teleportEffectStart(struct TeleportEffect* effect, int flags);

int teleportEffectUpdate(struct TeleportEffect* effect);
float teleportEffectGetHeight(float time);
float teleportEffectGetQuickLerp(struct TeleportEffect* effect);
void teleportEffectCreateTransform(struct TeleportEffect* effect, struct BasicTransform* transform, Mtx* mtx);

#endif