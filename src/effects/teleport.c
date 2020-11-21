
#include "teleport.h"

#include "src/time/time.h"
#include "src/math/mathf.h"

#define EFFECT_DURATION         3.0f
#define ANGULAR_ACCELERATION    5.0f
#define FALL_ACCEL              0.1f     

#define SCALE_ANIM_DURATION        0.125f
#define VERTICAL_SCALE_AMOUNT   2.5f
#define SCALE_START_TIME        (EFFECT_DURATION - SCALE_ANIM_DURATION)

void teleportEffectStart(struct TeleportEffect* effect, int flags) {
    if (flags & TELEPORT_FLAG_REVERSE) {
        effect->time = EFFECT_DURATION;
    } else {
        if (flags & TELEPORT_FLAG_QUICK) {
            effect->time = SCALE_START_TIME;
        } else {
            effect->time = 0.0f;
        }
    }

    effect->flags = TELEPORT_FLAG_ACTIVE | flags;
}

int teleportEffectUpdate(struct TeleportEffect* effect) {
    if (effect->flags & TELEPORT_FLAG_ACTIVE) {
        if (effect->flags & TELEPORT_FLAG_REVERSE) {
            effect->time -= gTimeDelta;

            if ((effect->flags & TELEPORT_FLAG_QUICK) && effect->time <= SCALE_START_TIME || 
                effect->time <= 0.0f) {
                effect->flags &= ~TELEPORT_FLAG_ACTIVE;
                return 0;
            }
        } else {
            effect->time += gTimeDelta;
            if (effect->time >= EFFECT_DURATION) {
                effect->flags &= ~TELEPORT_FLAG_ACTIVE;
                return 0;
            }
        }
    }

    return 1;
}

float teleportEffectGetHeight(float time) {
    return time * time * FALL_ACCEL;
}

void teleportEffectCreateTransform(struct TeleportEffect* effect, struct BasicTransform* transform, Mtx* mtx) {
    float tSqr;
    
    if (effect->flags & TELEPORT_FLAG_QUICK) {
        tSqr = 0;
    } else {
        tSqr = effect->time * effect->time;
    }

    float angle = tSqr * ANGULAR_ACCELERATION;
    float height = tSqr * FALL_ACCEL;
    
    struct BasicTransform relativeTransform;
    relativeTransform.position = gZeroVec;
    relativeTransform.position.y = height;
    quatAxisAngle(&gUp, angle, &relativeTransform.rotation);
    relativeTransform.scale = 1.0f;
    
    struct BasicTransform finalTransform;
    transformConcat(transform, &relativeTransform, &finalTransform);

    float fMtx[4][4];
    transformToMatrix(&finalTransform, 1.0f / 256.0f, fMtx);

    if (effect->time > SCALE_START_TIME) {
        float scaleLerp = (effect->time - SCALE_START_TIME) * (1.0f / SCALE_ANIM_DURATION);

        float hScale = 1.0f - scaleLerp;
        float vScale = mathfLerp(1.0f, VERTICAL_SCALE_AMOUNT, scaleLerp);

        fMtx[0][0] *= hScale; fMtx[0][1] *= hScale; fMtx[0][2] *= hScale;
        fMtx[1][0] *= vScale; fMtx[1][1] *= vScale; fMtx[1][2] *= vScale;
        fMtx[2][0] *= hScale; fMtx[2][1] *= hScale; fMtx[2][2] *= hScale;
    }

    guMtxF2L(fMtx, mtx);
}