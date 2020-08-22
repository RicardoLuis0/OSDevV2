#ifndef SHARED_PTR_H_INCLUDED
#define SHARED_PTR_H_INCLUDED

#include "stdc/stdatomic.h"
#include "klib.h"
#include "util/spinlock.h"
#include "util/tmp.h"
#include "util.h"

namespace Util {

    struct PtrData {
        inline PtrData():strong_ptr_count(1),weak_ptr_count(0){
        }
        Spinlock lock;
        atomic_uint_least32_t strong_ptr_count;
        atomic_uint_least32_t weak_ptr_count;
    };

    template<typename T>
    class SharedPtr {

        T * ptr;
        PtrData * ptrData;

        friend class WeakPrt;

        SharedPtr(PtrData * pd,T * t){
            ptr=t;
            ptrData=pd;
        }

    public:

        operator T*(){
            return ptr;
        }

        operator const T*() const{
            return ptr;
        }

        T* operator->(){
            return ptr;
        }

        SharedPtr():ptr(nullptr),ptrData(nullptr){
            
        }

        SharedPtr(nullptr_t):ptr(nullptr),ptrData(nullptr){
            
        }

        SharedPtr(T * t){//new shared pointer
            ptr=t;
            ptrData=new PtrData;
        }

        SharedPtr(const SharedPtr<T> &p){
            SpinlockGuard guard(ptrData->lock);
            ptr=p.ptr;
            ptrData=p.ptrData;
            ++(ptrData->strong_ptr_count);
        }

        SharedPtr(SharedPtr<T> && p){
            ptr=p.ptr;
            ptrData=p.ptrData;
            p.ptrData=nullptr;
            p.ptr=nullptr;
        }

        ~SharedPtr(){
            if(ptrData){
                SpinlockGuard guard(ptrData->lock);
                ptrData->strong_ptr_count--;
                if(ptrData->strong_ptr_count==0){
                    delete ptr;
                    if(ptrData->weak_ptr_count==0){
                        delete ptrData;
                    }
                }
            }
        }
    };

    template<typename T>
    class WeakPtr{//TODO improve

        T * ptr;
        PtrData * ptrData;

    public:
/*
        operator T*(){
            return ptr;
        }

        T* operator->(){
            return ptr;
        }
*/
        SharedPtr<T> get(){
            if(ptrData->strong_ptr_count>0){
                SpinlockGuard guard(ptrData->lock);
                ptrData->strong_ptr_count++;
                return SharedPtr<T>(ptrData,ptr);
            }else{
                return SharedPtr<T>();//since pointer isn't valid, return a null pointer, TODO replace by exception when they're implemented
            }
        }

        WeakPtr(const SharedPtr<T> &p){
            if(p.ptrData){
                SpinlockGuard guard(ptrData->lock);
                ptr=p.ptr;
                ptrData=p.ptrData;
                ++(ptrData->weak_ptr_count);
            }else{
                ptr=nullptr;
                ptrData=nullptr;
            }
        }

        ~WeakPtr(){
            if(ptrData){
                SpinlockGuard guard(ptrData->lock);
                ptrData->weak_ptr_count--;
                if(ptrData->strong_ptr_count==0&&ptrData->weak_ptr_count==0){
                    delete ptrData;
                }
            }
        }
    };

    template<typename T,typename... U>
    SharedPtr<T> makeShared(U&&... args){
        constexpr size_t ptr_start=align_size(sizeof(PtrData),alignof(T));
        void * buf=malloc(ptr_start+sizeof(T));
        return SharedPtr<T>(new(buf)PtrData,new((void*)(((uint8_t*)buf)+ptr_start))T(TMP::forward<U>(args)...));
    }

    template<typename T>
    class UniquePtr{
        
        T * ptr;
        
        UniquePtr(const UniquePtr<T>&)=delete;
        
        UniquePtr<T>&operator=(const UniquePtr<T>&)=delete;
        
    public:
        
        UniquePtr():ptr(nullptr){
        }
        
        UniquePtr(T * p):ptr(p){
        }
        
        UniquePtr(UniquePtr<T> && other){
            ptr=other.ptr;
            other.ptr=nullptr;
        }
        
        UniquePtr<T>&operator=(UniquePtr<T>&& other){
            if(ptr){
                delete ptr;
            }
            ptr=other.ptr;
            other.ptr=nullptr;
            return *this;
        }
        
        UniquePtr<T>& swap(UniquePtr<T> & other){
            T* p=other.ptr;
            other.ptr=ptr;
            ptr=p;
            return *this;
        }
        
        UniquePtr<T>&operator=(T * other){
            if(ptr){
                delete ptr;
            }
            ptr=other;
            return *this;
        }
        
        ~UniquePtr(){
            if(ptr){
                delete ptr;
            }
        }
        
        operator T*(){
            return ptr;
        }
        
        operator const T*() const{
            return ptr;
        }
        
        T* get(){
            return ptr;
        }
        
        const T* get() const{
            return ptr;
        }
        
        T*operator->(){
            return ptr;
        }
        
        const T* operator->() const{
            return ptr;
        }
        
        T* release(){
            T *p=ptr;
            ptr=nullptr;
            return p;
        }
        
        void reset(){
            delete ptr;
            ptr=nullptr;
        }
        
    };
    
    static_assert(sizeof(UniquePtr<void>)==sizeof(void*));//make sure unique_ptr doesn't have overhead
}

#endif // SHARED_PTR_H_INCLUDED
