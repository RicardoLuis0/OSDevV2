#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdatomic.h>
#include "util/tmp.h"

namespace Util {
    template<typename T,size_t N=20,bool discard_if_full=false>
    class LFQ {//fixed size lock-free queue
        volatile atomic_size_t location=0;
        volatile atomic_size_t size=0;
        uint8_t data_raw[N*sizeof(T)];
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
            data=reinterpret_cast<T*>(data_raw);
        }
        
        ~LFQ(){
            free(data);
        }
        
        T pop(){
            if(size==0){
                k_abort_s("trying to pop empty queue");
            }
            const size_t ol=location;
            adv_loc();
            size--;
            T tmp=TMP::move(data[ol]);
            data[ol].~T();
            return TMP::move(tmp);
        }
        
        void push(T && val){
            if(size<N){
                new(data+((location+size)%N))T(move(val));
                size++;
            }else if constexpr(discard_if_full){
                return;
            }else{
                const size_t ol=((location+size)%N);
                adv_loc();
                data[ol].~T();
                new(data+ol)T(move(val));
            }
        }
        
        void push(const T & val){
            if(size<N){
                new(data+((location+size)%N))T(val);
                size++;
            }else if constexpr(discard_if_full){
                return;
            }else{
                const size_t ol=location;
                adv_loc();
                data[ol].~T();
                new(data+ol)T(val);
            }
        }
        
        bool is_empty(){
            return size==0;
        }
        
    };
}
