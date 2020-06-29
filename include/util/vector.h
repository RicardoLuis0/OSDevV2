#ifndef VECTOR_H
#define VECTOR_H

#include "klib.h"
#include "util/spinlock.h"
#include "util/tmp.h"

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
        protected:
            Spinlock lock;
            size_t len;
            size_t alloc;
            T * vec;

        public:

            Vector():len(0),alloc(0),vec(nullptr){
            }


            Vector(const T * arr,size_t n){
                vec=(T*)calloc(sizeof(T),n);
                len=n;
                alloc=n;
                for(size_t i=0;i<n;i++){
                    vec[i]=arr[i];
                }
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
                        vec=(T*)realloc(vec,sizeof(T)*num);
                    }else{
                        vec=(T*)calloc(sizeof(T),num);
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

            T& front(){
                SpinlockGuard guard(lock);
                return vec[0];
            }

            T& back(){
                SpinlockGuard guard(lock);
                return vec[len-1];
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
                SpinlockGuard guard(lock);
                return len;
            }

            void shrink_to_fit(){
                SpinlockGuard guard(lock);
                if(alloc>len){
                    vec=(T*)realloc(vec,sizeof(T)*len);
                    alloc=len;
                }
            }

        protected:

            void grow(size_t min){
                size_t n=len+min;
                if(alloc>=n)return;//no need to grow
                if(vec){
                    if(n>(alloc*2)){
                        vec=(T*)realloc(vec,sizeof(T)*n);
                    }else{
                        vec=(T*)realloc(vec,sizeof(T)*alloc*2);//grow by doubling size
                    }
                }else{
                    vec=(T*)calloc(sizeof(T),n);
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
