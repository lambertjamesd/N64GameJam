
#ifndef _CADET_CADET_C
#define _CADET_CADET_C

#include "src/math/basictransform.h"
#include "src/time/time.h"

struct Cadet;

typedef void (*CadetState)(struct Cadet* cadet);

struct Cadet {
    struct BasicTransform transform;
    CadetState state;
    struct TimeUpdateListener updateListener;
};

extern struct Cadet gCadet;

void cadetInit();

#endif