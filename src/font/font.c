
#include "font.h"

void fontInit(struct Font* font, struct FontCharacter* characters, Gfx* setupFontMaterial, u8 lineHeight) {
    int i;

    for (i = 0; i < FONT_CHARACTER_COUNT; ++i) {
        font->ansiiLookup[i] = 0;
    }

    for (i = 0; characters[i].character; ++i) {
        font->ansiiLookup[characters[i].character] = &characters[i];
    }

    font->setupFontMaterial = setupFontMaterial;
    font->lineHeight = lineHeight;
}