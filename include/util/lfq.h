#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdatomic.h>
#include "util/tmp.h"

namespace Util {
    template<typename T,size_t N=20,bool discard_if_full=false>
    class LFQ {//fixed size lock-free queue
        atomic_size_t location=0;
        atomic_size_t size=0;
        T * data;
        LFQ(LFQ && other)=delete;
        LFQ& operator=(LFQ && other)=delete;
        LFQ(const LFQ & other)=delete;
        LFQ& operator=(const LFQ & other)=delete;
        inline __attribute__((always_inline)) void adv_loc() {
            location++;
            if(location==N){
                location=0;
            }
        }
    public:
        
        LFQ(){
            data=(T*)calloc(N,sizeof(T));
        }
        
        ~LFQ(){
            free(data);
        }
        
        T && pop(){
            if(size==0){
                k_abort_s("trying to pop empty queue");
            }
            const size_t ol=location;
            adv_loc();
            size--;
            T tmp=move(data[ol]);
            data[ol].~T();
            return tmp;
        }
        
        void push(T && val){
            if(size<N){
                new(data+location+size)T(move(val));
                size++;
            }else if constexpr(discard_if_full){
                return;
            }else{
                const size_t ol=location;
                adv_loc();
                data[ol].~T();
                new(data+ol)T(move(val));
            }
        }
        
    };
}
