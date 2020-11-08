
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

float mathfLerp(float from, float to, float t) {
    return from * (1.0f - t) + to * t;
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
    if (input >= 0 || input == (int)input) {
        return (int)input;
    } else {
        int result = (int)(input);
        return result - 1;
    }
}

float ceilf(float input) {
    if (input <= 0 || input == (int)input) {
        return (int)input;
    } else {
        int result = (int)(input);
        return result + 1;
    }
}