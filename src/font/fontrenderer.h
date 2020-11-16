
#ifndef FONT_FONTRENDERER_H
#define FONT_FONTRENDERER_H

#include <ultra64.h>
#include "font.h"

#define MAX_FONT_CHARACTERS     512 

struct FontRenderer {
    Bitmap bitmaps[MAX_FONT_CHARACTERS];
    Gfx dl[NUM_DL(MAX_FONT_CHARACTERS)];
    Sprite sprite;
};

void fontRendererBeginFrame(struct FontRenderer* renderer);
Gfx* fontRendererDrawCharacters(struct FontRenderer* fontRenderer, struct Font* font, Gfx* dl, char* string, int x, int y);

#endif