#ifndef VECTOR_H
#define VECTOR_H

#include "klib.h"
#include "util/spinlock.h"
#include "util/tmp.h"

namespace Util {
    
    template<typename T,auto VALUE_REMOVE=nullptr>//D is function called on removal of object instead of destructor
    class Vector{
            static void remove(T & t){
                if constexpr(TMP::is_null_pointer_dv<VALUE_REMOVE>){
                    t.~T();
                }else{
                    VALUE_REMOVE(t);
                }
            }
        protected:
            mutable Spinlock lock;
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
                    remove(vec[i]);
                }
                free(vec);
            }
            
            void push(const T &v){
                SpinlockGuard guard(lock);
                grow(1);
                new(&vec[len])T(v);
                len++;
            }
            
            void push(T && v){
                SpinlockGuard guard(lock);
                grow(1);
                new(&vec[len])T(TMP::move(v));
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
                        new(&vec[i])T(val);
                    }
                    len=num;
                }
            }
            
            void resize(size_t num){//resize vector and call default constructor
                SpinlockGuard guard(lock);
                if(len>num){//shrink
                    shrink(len-num);
                }else if(len<num){//grow
                    grow(num-len);
                    for(size_t i=len;i<num;i++){
                        new(&vec[i])T();
                    }
                    len=num;
                }
            }
            
            void erase(size_t elem){
                erase(elem,elem+1);
            }
            
            void erase(size_t first,size_t last){
                if(first>=len||first>last){
                    return;//invalid arguments
                }
                last=min(last,len);
                for(size_t i=first;i<last;i++){
                    remove(vec[i]);
                }
                size_t count=last-first;
                memmove(vec+first,vec+last,count*sizeof(T));
                len-=count;
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
            
            T& operator[](size_t i) const{
                SpinlockGuard guard(lock);
                return vec[i];
            }
            
            T& at(size_t i) const{
                SpinlockGuard guard(lock);
                return vec[i];
            }
            
            T& front() const{
                SpinlockGuard guard(lock);
                return vec[0];
            }
            
            T& back() const{
                SpinlockGuard guard(lock);
                return vec[len-1];
            }
            
            T* begin(){
                SpinlockGuard guard(lock);
                return len>0?&vec[0]:nullptr;
            }
            
            T* get(){
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
                    size_t n2=(alloc*1.5);
                    if(n<n2){
                        n=n2;//geometric growtn
                    }
                    vec=(T*)realloc(vec,sizeof(T)*n);
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
                    remove(vec[i]);
                }
                len=n;
            }
            
            
        private:
    };
}

#endif // VECTOR_H
