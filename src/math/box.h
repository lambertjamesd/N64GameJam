
#ifndef _MATH_BOX_H
#define _MATH_BOX_H

#include "vector.h"

struct Box {
    struct Vector3 min, max;
};

#endif