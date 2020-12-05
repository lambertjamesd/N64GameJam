
#include <ultra64.h>
#include "fontrenderer.h"
#include "src/defs.h"
#include "src/system/assert.h"
#include "src/graphics/graphics.h"

void fontRendererBeginFrame(struct FontRenderer* renderer) {
    renderer->nextDL = renderer->dl;

    renderer->sx = 1.0f;
    renderer->sy = 1.0f;
}

void fontRendererSetScale(struct FontRenderer* fontRenderer, float x, float y) {
    fontRenderer->sx = x;
    fontRenderer->sy = y;
}

float fontRendererDrawCharacters(struct FontRenderer* fontRenderer, struct Font* font, Gfx** dlPtr, char* string, int x, int y) {
    Gfx* nextDL = fontRenderer->nextDL;

    float currX = x;
    int shiftedY = y << 2;
    int shiftedLower = shiftedY + (int)(font->lineHeight * fontRenderer->sy * 4.0f);

    int deltaTextureX = (int)((float)(1 << 10) / fontRenderer->sx);
    int deltaTextureY = (int)((float)(1 << 10) / fontRenderer->sy);

    int topOffset = 0;

    if (shiftedY < 0) {
        topOffset = (-shiftedY) << 3;
        topOffset = 0;
    }

    if (shiftedLower > (gScreenHeight << 2)) {
        shiftedLower = gScreenHeight << 2;
    }

    if (shiftedLower <= shiftedY) {
        return x + fontRendererMeasureWidth(font, string) * fontRenderer->sx;
    }

    while (*string) {
        struct FontCharacter* character = font->ansiiLookup[*string];

        if (character) {
            float renderX = currX;
            float renderRight = renderX + character->w * fontRenderer->sx;
            float s = character->left;
            float t = character->top;
            int leftOffset = 0;

            if (renderX < 0) {
                leftOffset = (int)(renderX * -32.0f);
                renderX = 0.0f;
            }

            if (renderRight > SCREEN_WD) {
                renderRight = SCREEN_WD;
            }

            int renderXInt = (int)(renderX * 4.0f);
            int renderRightInt = (int)(renderRight * 4.0f);

            if (renderXInt < renderRightInt) {
                assert((nextDL-fontRenderer->dl) < MAX_FONT_CHARACTERS);
                gSPTextureRectangle(
                    nextDL++,
                    renderXInt, shiftedY,
                    renderRightInt, shiftedLower,
                    0,
                    (character->left << 5) + leftOffset, (character->top << 5) + topOffset,
                    deltaTextureX, deltaTextureY
                );
            }

            currX += (character->w + character->kerning) * fontRenderer->sx;
        }

        ++string;
    }

    assert((nextDL-fontRenderer->dl) < MAX_FONT_CHARACTERS);
    gSPEndDisplayList(nextDL++);
    gSPDisplayList((*dlPtr)++, fontRenderer->nextDL);

    fontRenderer->nextDL = nextDL;

    return currX;
}

float fontRendererMeasureWidth(struct Font* font, char* string) {
    float result = 0.0f;

    while (*string) {
        struct FontCharacter* character = font->ansiiLookup[*string];

        if (character) {
            result += character->w;
        }

        ++string;
    }

    return result;
}