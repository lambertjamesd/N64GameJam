
#ifndef _FONT_BUTTONS_H
#define _FONT_BUTTONS_H

#include "src/font/font.h"

enum ButtonFontMapping {
    ButtonFontMappingNone,
    ButtonFontMappingA,
    ButtonFontMappingB,
    ButtonFontMappingC_U,
    ButtonFontMappingC_R,
    ButtonFontMappingC_L,
    ButtonFontMappingC_D,
    ButtonFontMappingZ,
    ButtonFontMappingJoystick,
    ButtonFontMappingGemGone,
    ButtonFontMappingGem,
    ButtonFontMappingStart,
    ButtonFontMappingR,
};

extern u8 _buttons_font_rgba16[];
extern struct FontCharacter gButtonFontCharacters[];

extern struct Font gButtonFont;
extern Gfx gButtonFontUse[];

#endif