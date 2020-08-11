#pragma once
namespace TEdit {
    template<typename T> constexpr T max(T a,T b){
        return a>b?a:b;
    }
    
    template<typename T> constexpr T min(T a,T b){
        return a>b?b:a;
    }
}
