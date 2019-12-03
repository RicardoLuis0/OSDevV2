#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

template<typename T> constexpr T min(T a,T b){return a<b?a:b;}
template<typename T> constexpr T max(T a,T b){return a>b?a:b;}
template<typename T> constexpr T clamp(T a,T b,T c){return c<a?a:c>b?b:c;}

#endif // UTIL_H_INCLUDED
