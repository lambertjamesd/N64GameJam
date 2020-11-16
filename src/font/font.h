
#ifndef FONT_FONT_H
#define FONT_FONT_H

#include <ultra64.h>

#define FONT_CHARACTER_COUNT    128

struct FontCharacter {
    char character;
    char left, top, w;
};

struct Font {
    struct FontCharacter* ansiiLookup[FONT_CHARACTER_COUNT];
    u8* fontImage;
	u8	bitmapFormat;
	u8	texelSize;
    u8 imageWidth;
    u8 imageHeight;
    u8 lineHeight;
};

void fontInit(struct Font* font, struct FontCharacter* characters, u8* image, u8 bitmapFormat, u8 texelSize, u8 imageWidth, u8 imageHeight, u8 lineHeight);

#endif