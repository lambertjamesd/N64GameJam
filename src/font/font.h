
#ifndef FONT_FONT_H
#define FONT_FONT_H

#include <ultra64.h>

#define FONT_CHARACTER_COUNT    128

struct FontCharacter {
    char character;
    char left, top, w;
    char kerning;
    char yOffset;
};

struct Font {
    struct FontCharacter* ansiiLookup[FONT_CHARACTER_COUNT];
    Gfx* setupFontMaterial;
    u8 lineHeight;
};

void fontInit(struct Font* font, struct FontCharacter* characters, Gfx* setupFontMaterial, u8 lineHeight);

#endif