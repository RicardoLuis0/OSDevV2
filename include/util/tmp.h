#ifndef TMPLIB_H_INCLUDED
#define TMPLIB_H_INCLUDED

#include "stdc/stddef.h"

#include "util/tmp/base.h"
#include "util/tmp/is_function.h"

namespace TMP {
    
    template< class T >
    struct is_member_function_pointer_helper : false_type {};
    
    template<class T, class U>
    struct is_member_function_pointer_helper<T U::*> : is_function<T> {};
    
    template<class T>
    struct is_member_function_pointer : is_member_function_pointer_helper< typename remove_cv<T>::type > {};
    
}

#endif // TMPLIB_H_INCLUDED
