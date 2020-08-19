#ifndef TMPLIB_IS_FUNCTION_H_INCLUDED
#define TMPLIB_IS_FUNCTION_H_INCLUDED

#include "util/tmp/base.h"

namespace TMP {
    
    // primary template
    template<class>
    struct is_function : false_type { };
     
    // specialization for regular functions
    template<class Ret, class... Args>
    struct is_function<Ret(Args...)> : true_type {};
     
    // specialization for variadic functions such as printf
    template<class Ret, class... Args>
    struct is_function<Ret(Args...,...)> : true_type {};
     
    // specialization for function types that have cv-qualifiers
    template<class Ret, class... Args>
    struct is_function<Ret(Args...) const> : true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args...) volatile> : true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args...) const volatile> : true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args...,...) const> : true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args...,...) volatile> : true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args...,...) const volatile> : true_type {};
     
    // specialization for function types that have ref-qualifiers
    template<class Ret, class... Args>
    struct is_function<Ret(Args...) &> : true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args...) const &> : true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args...) volatile &> : true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args...) const volatile &> : true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args...,...) &> : true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args...,...) const &> : true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args...,...) volatile &> : true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args...,...) const volatile &> : true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args...) &&> : true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args...) const &&> : true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args...) volatile &&> : true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args...) const volatile &&> : true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args...,...) &&> : true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args...,...) const &&> : true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args...,...) volatile &&> : true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args...,...) const volatile &&> : true_type {};
     
    // specializations for noexcept versions of all the above (C++17 and later)
     
    template<class Ret, class... Args>
    struct is_function<Ret(Args...) noexcept> : true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args...,...) noexcept> : true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args...) const noexcept> : true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args...) volatile noexcept> : true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args...) const volatile noexcept> : true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args...,...) const noexcept> : true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args...,...) volatile noexcept> : true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args...,...) const volatile noexcept> : true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args...) & noexcept> : true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args...) const & noexcept> : true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args...) volatile & noexcept> : true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args...) const volatile & noexcept> : true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args...,...) & noexcept> : true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args...,...) const & noexcept> : true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args...,...) volatile & noexcept> : true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args...,...) const volatile & noexcept> : true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args...) && noexcept> : true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args...) const && noexcept> : true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args...) volatile && noexcept> : true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args...) const volatile && noexcept> : true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args...,...) && noexcept> : true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args...,...) const && noexcept> : true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args...,...) volatile && noexcept> : true_type {};
    template<class Ret, class... Args>
    struct is_function<Ret(Args...,...) const volatile && noexcept> : true_type {};
}

#endif // TMPLIB_IS_FUNCTION_H_INCLUDED
