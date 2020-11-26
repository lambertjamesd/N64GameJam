
#ifndef FONT_ENDLESSBOSSBATTLE
#define FONT_ENDLESSBOSSBATTLE

#include "src/font/font.h"

enum EBBSpecial {
    EBBUpArrow = 1,
    EBBDownArrow = 2,
};

extern u8 _endless_boss_battle_ia4[];
extern struct FontCharacter gEndlessBossBattleCharacters[];

extern struct Font gEndlessBossBattle;
extern Gfx gEndlessBossBattleUse[];

#endif