
#include "mathf.h"
#include <math.h>


float fsign(float in) {
    if (in > 0.0f) {
        return 1.0f;
    } else if (in < 0.0f) {
        return -1.0f;
    } else {
        return 0.0f;
    }
}

float mathfMoveTowards(float from, float to, float maxMove) {
    float offset = to - from;
    if (fabs(offset) <= maxMove) {
        return to;
    } else {
        return fsign(offset) * maxMove + from;
    }
}

float floorf(float input) {
    int result;

    if (input > 0) {
        result = (int)input;
    } else {
        result = (int)-input;
        input = -input;
    }

    return (float)result;
}

float ceilf(float input) {
    int result;

    if (input < 0) {
        result = (int)input;
    } else {
        result = (int)-input;
        input = -input;
    }

    return (float)result;
}