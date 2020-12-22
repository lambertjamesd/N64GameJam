
#ifndef FONT_FONTRENDERER_H
#define FONT_FONTRENDERER_H

#include "font.h"

#define MAX_FONT_CHARACTERS     700 

struct FontRenderer {
    Gfx dl[MAX_FONT_CHARACTERS];
    Gfx* nextDL;
    float sx, sy;
};

void fontRendererBeginFrame(struct FontRenderer* renderer);

void fontRendererSetScale(struct FontRenderer* fontRenderer, float x, float y);

float fontRendererDrawCharacters(struct FontRenderer* fontRenderer, struct Font* font, Gfx** dlPtr, char* string, int x, int y);
float fontRendererMeasureWidth(struct Font* font, char* string);


#endif