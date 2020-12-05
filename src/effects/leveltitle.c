
#include "leveltitle.h"

#include "src/graphics/graphics.h"
#include "src/font/fontrenderer.h"
#include "src/font/endlessbossbattle/endlessbossbattle.h"
#include "src/font/buttons/buttons.h"
#include "src/defs.h"

#define MESSAGE_SCALE   2.0f

#define MESSAGE_ENTER_TIME      1.0f
#define BAR_SCALE_TIME          0.25f
#define MESSAGE_TOTAL_TIME      3.0f
#define MESSAGE_SLIDE_DISTANCE  60.0f
#define MESSAGE_OFFSCREEN_BUFFER    20.0f

#define BAR_HEIGHT  30

#define MESSAGE_SLIDE_VELOCITY (MESSAGE_SLIDE_DISTANCE/(MESSAGE_TOTAL_TIME-MESSAGE_ENTER_TIME*2))

struct LevelTitleEffect gLevelTitleEffect;

void levelTitleEffectRender(void* data, struct GraphicsState* state, struct FontRenderer* fontRenderer) {
    struct LevelTitleEffect* titleEffect = (struct LevelTitleEffect*)data;
    
    int barHeight = 0;

    if (titleEffect->currTime < BAR_SCALE_TIME) {
        barHeight = (int)SCALE_FOR_PAL(BAR_HEIGHT * (titleEffect->currTime / BAR_SCALE_TIME));
    } else if (titleEffect->currTime > MESSAGE_TOTAL_TIME - BAR_SCALE_TIME) {
        float lerpTime = titleEffect->currTime - MESSAGE_TOTAL_TIME + BAR_SCALE_TIME;
        lerpTime = 1.0f - lerpTime/ BAR_SCALE_TIME;
        barHeight = (int)SCALE_FOR_PAL(BAR_HEIGHT * lerpTime);
    } else {
        barHeight = SCALE_FOR_PAL(BAR_HEIGHT);
    }

    int barY = (gScreenHeight >> 1) + 40;

    if (barHeight > 0) {
        gDPPipeSync(state->dl++);
        gDPSetCycleType(state->dl++, G_CYC_FILL);
        gDPSetFillColor(state->dl++, (GPACK_RGBA5551(0, 0, 0, 1) << 16 | 
                    GPACK_RGBA5551(0, 0, 0, 1)));
        gDPFillRectangle(state->dl++, 0, barY-(barHeight>>1), SCREEN_WD-1, barY+(barHeight>>1));
        gDPPipeSync(state->dl++);


        float centerOffset = (titleEffect->currTime - MESSAGE_TOTAL_TIME*0.5f) * MESSAGE_SLIDE_VELOCITY;

        if (titleEffect->currTime < MESSAGE_ENTER_TIME) {
            float accelTime = MESSAGE_ENTER_TIME - titleEffect->currTime;
            centerOffset += titleEffect->edgeAccel * accelTime * accelTime;
        } else if (titleEffect->currTime > MESSAGE_TOTAL_TIME - MESSAGE_ENTER_TIME) {
            float accelTime = titleEffect->currTime - (MESSAGE_TOTAL_TIME - MESSAGE_ENTER_TIME);
            centerOffset -= titleEffect->edgeAccel * accelTime * accelTime;
        }

        gSPDisplayList(state->dl++, gEndlessBossBattleUse);
        fontRendererSetScale(fontRenderer, MESSAGE_SCALE, MESSAGE_SCALE * gScreenYScale);
        fontRendererDrawCharacters(
            fontRenderer, 
            &gEndlessBossBattle, 
            &state->dl, 
            titleEffect->message, 
            -titleEffect->titleWidth * 0.5f + SCREEN_WD*0.5f+centerOffset, 
            barY-SCALE_FOR_PAL(8)
        );

        gDPPipeSync(state->dl++);
    }
}

void levelTitleEffectUpdate(void* data) {
    struct LevelTitleEffect* titleEffect = (struct LevelTitleEffect*)data;
    titleEffect->currTime += gTimeDelta;

    if (titleEffect->currTime > MESSAGE_TOTAL_TIME) {
        timeRemoveListener(&titleEffect->updateListener, TimeUpdateGroupWorld);
        graphicsRemoveMenu(levelTitleEffectRender, titleEffect);
    }
}

void levelTitleEffectInit(struct LevelTitleEffect* titleEffect, char* message) {
    titleEffect->currTime = 0.0f;
    titleEffect->message = message;
    titleEffect->titleWidth = fontRendererMeasureWidth(&gEndlessBossBattle, message) * MESSAGE_SCALE;

    float dx = (SCREEN_WD - MESSAGE_SLIDE_DISTANCE) * 0.5f + MESSAGE_OFFSCREEN_BUFFER + titleEffect->titleWidth/2;
    // slideV = MESSAGE_SLIDE_DISTANCE/(MESSAGE_TOTAL_TIME-MESSAGE_ENTER_TIME*2)
    // dx = MESSAGE_ENTER_TIME*slideV+0.5*a*MESSAGE_ENTER_TIME*MESSAGE_ENTER_TIME

    titleEffect->edgeAccel = (MESSAGE_ENTER_TIME*MESSAGE_SLIDE_VELOCITY-dx)/(MESSAGE_ENTER_TIME*MESSAGE_ENTER_TIME);

    graphicsAddMenu(levelTitleEffectRender, titleEffect, 0);
    timeAddListener(&titleEffect->updateListener, levelTitleEffectUpdate, titleEffect, TimeUpdateGroupWorld);
}