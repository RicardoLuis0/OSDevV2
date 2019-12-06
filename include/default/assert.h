#ifndef ASSERT_H_INCLUDED
#define ASSERT_H_INCLUDED

#include "klib.h"

#define assert(expr) if(!(expr)){k_abort_assert(#expr,__FILE__,__LINE__);}
#define fassert(expr) if(!(expr)){k_abort_assert(#expr,__func__,__LINE__);}

#endif // ASSERT_H_INCLUDED
