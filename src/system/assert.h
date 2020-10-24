
#ifndef _ASSERT_H
#define _ASSERT_H

#ifdef DEBUG
    void assert(int assertion);
#else
    #define assert(assertion)
#endif

#endif