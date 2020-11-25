
#include <ultra64.h>
#include "spinninglogo.inc.c"
#include "fresheaters.inc.c"
#include "src/font/font.h"

struct FontCharacter gFreshEaterCharacters[] = {
    {'U',   1, 0, 17, 2},
    {'L',   19, 0, 15, -3},
    {'T',   40, 0, 16, 1},
    {'A',   62, 0, 18, 2},
    {'R',   86, 0, 17, 1},
    {'E',   109, 0, 15},

    {' ',   34, 0, 5},

    {'\0',   0, 0, 0},
};

struct Font gFreshEater;