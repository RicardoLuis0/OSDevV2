#ifndef VECTOR_H
#define VECTOR_H

#include "klib.h"
#include "util/spinlock.h"

namespace Util {

    template<typename T,auto RV> struct REMOVE {
        static void remove(T&t){
            RV(t);
        }
    };

    template<typename T> struct REMOVE<T,nullptr> {
        static void remove(T&t){
            t.~T();
        }
    };

    template<typename T,auto VALUE_REMOVE=nullptr>//D is function called on removal of object instead of destructor
    class Vector{
            Spinlock lock;
            size_t len;
            size_t alloc;
            T * vec;

        public:

            Vector():len(0),alloc(0),vec(nullptr){
            }

            virtual ~Vector(){
                for(size_t i=0;i<len;i++){
                    REMOVE<T,VALUE_REMOVE>::remove(vec[i]);
                }
                free(vec);
            }

            void push(T v){
                SpinlockGuard guard(lock);
                grow(1);
                vec[len]=v;
                len++;
            }

            void reserve(size_t num){//reserve space for [num] new elements
                SpinlockGuard guard(lock);
                if(alloc<num){
                    if(vec){
                        vec=(T*)k_realloc(vec,sizeof(T)*num);
                    }else{
                        vec=(T*)k_calloc(sizeof(T),num);
                    }
                    alloc=num;
                }
            }

            void resize(size_t num,T val){//resize vector and set any new elements to val
                SpinlockGuard guard(lock);
                if(len>num){//shrink
                    shrink(len-num);
                }else if(len<num){//grow
                    grow(num-len);
                    for(size_t i=len;i<num;i++){
                        vec[i]=val;
                    }
                    len=num;
                }
            }

            T& operator[](size_t i){
                SpinlockGuard guard(lock);
                return vec[i];
            }

            T& at(size_t i){
                SpinlockGuard guard(lock);
                return vec[i];
            }

            T* begin(){
                SpinlockGuard guard(lock);
                return len>0?&vec[0]:nullptr;
            }

            T* end(){
                SpinlockGuard guard(lock);
                return len>0?&vec[len]:nullptr;
            }

            size_t size(){
                return len;
            }

            void shrink_to_fit(){
                SpinlockGuard guard(lock);
                if(alloc>len){
                    vec=(T*)k_realloc(vec,sizeof(T)*len);
                    alloc=len;
                }
            }

        protected:

            void grow(size_t by){
                size_t n=len+by;
                if(alloc>=n)return;//no need to grow
                if(vec){
                    vec=(T*)k_realloc(vec,sizeof(T)*n);
                }else{
                    vec=(T*)k_calloc(sizeof(T),n);
                }
                alloc=n;
            }

            void shrink(size_t by){
                size_t n;
                if(by>len){
                    n=0;
                }else{
                    n=len-by;
                }
                for(size_t i=n;i<len;i++){
                    REMOVE<T,VALUE_REMOVE>::remove(vec[i]);
                }
                len=n;
            }

        private:
    };
}

#endif // VECTOR_H
