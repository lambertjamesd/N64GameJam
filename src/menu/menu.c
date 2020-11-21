
#include "menu.h"

int menuGetInputDir(char axis, char* lastAxis) {
    int result = 0;

    if (axis < -40 && *lastAxis >= -40) {
        result = -1;
    } else if (axis > 40 && *lastAxis <= 40) {
        result = 1;
    }

    *lastAxis = axis;

    return result;
}