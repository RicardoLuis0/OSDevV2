#ifndef ASSERT_H_INCLUDED
#define ASSERT_H_INCLUDED

#include "klib.h"

//assert is default assert
//fassert uses function name instead of file name

#ifndef DEBUG

#define assert(condition) ((void)0)
#define fassert(condition) ((void)0)

#else

#define assert(condition) if(!(condition)){k_abort_assert(#condition,__FILE__,__LINE__);}
#define fassert(condition) if(!(condition)){k_abort_assert(#condition,__func__,__LINE__);}

#endif // DEBUG

#endif // ASSERT_H_INCLUDED
