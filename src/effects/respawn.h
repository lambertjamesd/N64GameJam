
#ifndef _EFFECTS_RESPAWN_H
#define _EFFECTS_RESPAWN_H

#include "src/graphics/graphics.h"
#include "teleport.h"

struct RespawnEffect {
    struct TeleportEffect *teleportEffect;
    float scale;
    int renderId;
};

void respawnEffectStart(struct RespawnEffect* effect, struct BasicTransform* transform, float scale, struct TeleportEffect* teleport);
void respawnEffectEnd(struct RespawnEffect* effect);

#endif