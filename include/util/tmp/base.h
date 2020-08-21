#ifndef TMPLIB_BASE_H_INCLUDED
#define TMPLIB_BASE_H_INCLUDED



#include "stdc/stddef.h"

namespace TMP {
    
    struct true_type {static constexpr bool value = true;};
    
    struct false_type {static constexpr bool value = false;};
    
    template<typename T> struct type_container{using type=T;};
    
    template<typename T> struct remove_const:type_container<T>{};
    template<typename T> struct remove_const<const T>:type_container<T>{};
    
    template<typename T> struct remove_volatile:type_container<T>{};
    template<typename T> struct remove_volatile<volatile T>:type_container<T>{};
    
    template<typename T> struct remove_cv:type_container<typename remove_const<typename remove_volatile<T>::type>::type>{};
    
    template<typename T> struct remove_reference:type_container<T>{};
    template<typename T> struct remove_reference<T&>:type_container<T>{};
    template<typename T> struct remove_reference<T&&>:type_container<T>{};
    
    template<typename T,typename U> struct is_same:false_type{};
    
    template<typename T> struct is_same<T,T>:true_type{};
    
    template<typename T> struct is_null_pointer:is_same<typename remove_cv<T>::type,nullptr_t>{};
    
    template<typename> struct is_pointer:false_type{};
    template<typename T> struct is_pointer<T*>:true_type{};
    
    template<typename T>
    constexpr bool is_null_pointer_v = is_null_pointer<T>::value;
    
    template<typename T>
    constexpr bool is_pointer_v = is_pointer<T>::value;
    
    template<auto V>
    constexpr bool is_null_pointer_dv = is_null_pointer<decltype(V)>::value;
    
    template<typename T>
    constexpr T&& forward(typename remove_reference<T>::type& t) noexcept {
        return static_cast<T&&>(t);
    }
    
    template <typename T>
    constexpr T&& forward(typename remove_reference<T>::type&& t) noexcept {
        return static_cast<T&&>(t);
    }
    
    template <typename T>
    typename remove_reference<T>::type&& move(T&& t) {
      return static_cast<typename remove_reference<T>::type&&>(t);
    }
    
    template<typename T> struct remove_pointer:type_container<T>{};
    template<typename T> struct remove_pointer<T*>:type_container<T>{};
    template<typename T> struct remove_pointer<T* const>:type_container<T>{};
    template<typename T> struct remove_pointer<T* volatile>:type_container<T>{};
    template<typename T> struct remove_pointer<T* const volatile>:type_container<T>{};
    
    template<bool,typename T=void> struct enable_if {};
    template<typename T> struct enable_if<true, T> {using type=T;};
    
    template<bool B,typename T,typename F> struct conditional{using type=T;};
    template<typename T,typename F> struct conditional<false,T,F>{using type=F;};
    
    template<class...> struct conjunction : true_type {};
    template<class T> struct conjunction<T> : T {};
    template<class T,class... Tn> struct conjunction<T, Tn...> : conditional<T::value, conjunction<Tn...>, T> {};
    
}

#endif // TMPLIB_BASE_H_INCLUDED
