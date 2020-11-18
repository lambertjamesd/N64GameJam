
#ifndef FONT_FONTRENDERER_H
#define FONT_FONTRENDERER_H

#include "font.h"

#define MAX_FONT_CHARACTERS     512 

struct FontRenderer {
    Bitmap bitmaps[MAX_FONT_CHARACTERS];
    Gfx dl[NUM_DL(MAX_FONT_CHARACTERS)];
    Sprite sprite;
    float sx, sy;
    u8 r, g, b, a;
};

void fontRendererBeginFrame(struct FontRenderer* renderer);

void fontRendererSetScale(struct FontRenderer* fontRenderer, float x, float y);
void fontRendererSetColor(struct FontRenderer* fontRenderer, u8 red, u8 green, u8 blue, u8 alpha);

Gfx* fontRendererDrawCharacters(struct FontRenderer* fontRenderer, struct Font* font, Gfx* dl, char* string, int x, int y);
float fontRendererMeasureWidth(struct Font* font, char* string);


#endif