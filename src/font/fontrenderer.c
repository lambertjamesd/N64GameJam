
#include <ultra64.h>
#include "fontrenderer.h"

void fontRendererBeginFrame(struct FontRenderer* renderer) {
    renderer->sprite.x = 0;
    renderer->sprite.y = 0;
    renderer->sprite.width = 0;
    renderer->sprite.height = 0;
    renderer->sprite.scalex = 1.0f;
    renderer->sprite.scaley = 1.0f;
    renderer->sprite.expx = 0;
    renderer->sprite.expy = 0;
    renderer->sprite.attr = SP_TRANSPARENT|SP_CUTOUT;
    renderer->sprite.zdepth = 0x1234;
    renderer->sprite.red = 255;
    renderer->sprite.green = 255;
    renderer->sprite.blue = 255;
    renderer->sprite.alpha = 255;
    renderer->sprite.startTLUT = 0;
    renderer->sprite.nTLUT = 0;
    renderer->sprite.LUT = 0;
    renderer->sprite.istart = 0;
    renderer->sprite.istep = 1;
    renderer->sprite.nbitmaps = 0;
    renderer->sprite.ndisplist = NUM_DL(MAX_FONT_CHARACTERS);
    renderer->sprite.bmheight = 8;
    renderer->sprite.bmHreal = 64;
    renderer->sprite.bmfmt = G_IM_FMT_I;
    renderer->sprite.bmsiz = G_IM_SIZ_4b;
    renderer->sprite.bitmap = renderer->bitmaps;
    renderer->sprite.rsp_dl = renderer->dl;
    renderer->sprite.rsp_dl_next = renderer->dl;
    renderer->sprite.frac_s = 0;
    renderer->sprite.frac_t = 0;

    renderer->sx = 1.0f;
    renderer->sy = 1.0f;

    renderer->r = 255;
    renderer->g = 255;
    renderer->b = 255;
    renderer->a = 255;
}

void fontRendererSetScale(struct FontRenderer* fontRenderer, float x, float y) {
    fontRenderer->sx = x;
    fontRenderer->sy = y;
}

void fontRendererSetColor(struct FontRenderer* fontRenderer, u8 red, u8 green, u8 blue, u8 alpha) {
    fontRenderer->r = red;
    fontRenderer->g = green;
    fontRenderer->b = blue;
    fontRenderer->a = alpha;
}

Gfx* fontRendererDrawCharacters(struct FontRenderer* fontRenderer, struct Font* font, Gfx* dl, char* string, int x, int y) {
    fontRenderer->sprite.bmsiz = font->texelSize;
    fontRenderer->sprite.bmfmt = font->bitmapFormat;
    fontRenderer->sprite.bmHreal = font->imageHeight;
    fontRenderer->sprite.bmheight = font->imageHeight;

    fontRenderer->sprite.height = font->lineHeight;
    int width = 0;
    int charCount = 0;

    while (*string) {
        struct FontCharacter* character = font->ansiiLookup[*string];

        if (character) {
            Bitmap* bitmap = &fontRenderer->bitmaps[charCount];

            bitmap->actualHeight = font->lineHeight;
            bitmap->buf = font->fontImage;
            bitmap->LUToffset = 0;
            bitmap->s = character->left;
            bitmap->t = character->top;
            bitmap->width = character->w;
            bitmap->width_img = font->imageWidth;

            width += character->w;
            ++charCount;
        }

        ++string;
    }

    fontRenderer->sprite.nbitmaps = charCount;
    fontRenderer->sprite.width = width;

    spMove(&fontRenderer->sprite, x, y);
    spScale(&fontRenderer->sprite, fontRenderer->sx, fontRenderer->sy);
    spColor(&fontRenderer->sprite, fontRenderer->r, fontRenderer->g, fontRenderer->b, fontRenderer->a);
    Gfx* renderBatch = spDraw(&fontRenderer->sprite);
    gSPDisplayList(dl++, renderBatch);
    return dl;
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