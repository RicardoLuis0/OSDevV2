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

public:

        operator T*(){
            return ptr;
        }

        T* operator->(){
            return ptr;
        }

        SharedPtr(nullptr_t):ptr(nullptr),ptrData(nullptr){
            
        }

        SharedPtr(PtrData * pd,T * t){
            ptr=t;
            ptrData=pd;
        }

        SharedPtr(T * t){//new shared pointer
            ptr=t;
            ptrData=new(k_malloc(sizeof(PtrData)))PtrData;
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
                    k_free(ptr);
                    if(ptrData->weak_ptr_count==0){
                        k_free(ptrData);
                    }
                }
            }
        }

    };

    template<typename T>
    class WeakPtr{

        T * ptr;
        PtrData * ptrData;

    public:

        operator T*(){
            return ptr;
        }

        T* operator->(){
            return ptr;
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
                    k_free(ptrData);
                }
            }
        }

    };

    template<typename T,typename... U>
    SharedPtr<T> makeShared(U&&... args){
        constexpr size_t ptr_start=align_size(sizeof(PtrData),alignof(T));
        void * buf=k_malloc(ptr_start+sizeof(T));
        return SharedPtr<T>(new(buf)PtrData,new((void*)(((uint8_t*)buf)+ptr_start))T(TMP::forward<U>(args)...));
    }

}

#endif // SHARED_PTR_H_INCLUDED
