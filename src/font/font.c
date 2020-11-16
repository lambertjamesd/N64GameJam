
#include "font.h"

void fontInit(struct Font* font, struct FontCharacter* characters, u8* image, u8 bitmapFormat, u8 texelSize, u8 imageWidth, u8 imageHeight, u8 lineHeight) {
    int i;

    for (i = 0; i < FONT_CHARACTER_COUNT; ++i) {
        font->ansiiLookup[i] = 0;
    }

    for (i = 0; characters[i].character; ++i) {
        font->ansiiLookup[characters[i].character] = &characters[i];
    }

    font->fontImage = image;
    font->bitmapFormat = bitmapFormat;
    font->texelSize = texelSize;
    font->imageWidth = imageWidth;
    font->imageHeight = imageHeight;
    font->lineHeight = lineHeight;
}