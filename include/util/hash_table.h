#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "util/vector.h"
#include "util/tmp.h"
#include "klib.h"
#include "string.h"

namespace Util {
    
    template<typename T>
    size_t Hash(const T&){
        static_assert(sizeof(T) == 0,"Hash function unimplemented for type");
    }
    
    template<>
    inline size_t Hash<const Util::String &>(const Util::String &s){
        return k_hash_s(s.c_str());
    }
    
    //if you change key type, you need to also change the compare,remove,hash,clone functions
    template<typename T,                        //type
             typename K=String,                 //internal key type
             size_t N=256,                      //size of hash table, higher size means lower collision, but increases memory usage
             typename CK=const K &,             //input key type
             auto KEY_HASH=Hash<CK>,            //key hash function, required
             auto KEY_COMPARE=nullptr           //key comparsion function, required for pointers, if nullptr will use '=='
            >
    class HashTable {
            //asserts
            static_assert(TMP::is_function<typename TMP::remove_pointer<decltype(KEY_HASH)>::type>::value,"KEY_HASH must be a function");
            static_assert(TMP::is_null_pointer_dv<KEY_COMPARE>||TMP::is_function<typename TMP::remove_pointer<decltype(KEY_COMPARE)>::type>::value,"KEY_COMPARE must be nullptr or a function");
            
            Spinlock lock;
            
            static bool compare(K&k,CK&ck){
                if constexpr(TMP::is_null_pointer_dv<KEY_COMPARE>){
                    return (k==ck);
                }else{
                    return KEY_COMPARE(k,ck);
                }
            }
            
            struct Value {
                Value(CK k2,const T &v2){//copy
                    k=k2;
                    v=v2;
                }
                
                Value(CK k2,T && v2){//move
                    k=k2;
                    v=TMP::move(v2);
                }
                
                Value(const Value &other){
                    k=other.k;
                    v=other.v;
                }
                
                Value(Value && other){
                    k=TMP::move(other.k);
                    v=TMP::move(other.v);
                }
                
                K k;
                T v;
            };
            Util::Vector<Value> ht[N];
        public:
            HashTable(){
            }
            
            bool has(CK k){
                SpinlockGuard guard(lock);
                auto & vec=ht[KEY_HASH(k)%N];
                for(Value &v:vec){
                    if(compare(v.k,k)){
                        return true;
                    }
                }
                return false;
            }
            
            T& operator[](CK k){
                return at(k);
            }
            
            T& at(CK k){
                SpinlockGuard guard(lock);
                auto & vec=ht[KEY_HASH(k)%N];
                for(Value &v:vec){
                    if(compare(v.k,k)){
                        return v.v;
                    }
                }
                vec.push_back(Value(k,T()));
                return vec.back().v;
            }
            
            T * find(CK k){//if found return pointer, else null
                SpinlockGuard guard(lock);
                auto & vec=ht[KEY_HASH(k)%N];
                for(Value &v:vec){
                    if(compare(v.k,k)){
                        return &v.v;
                    }
                }
                return nullptr;
            }
            
            void set(CK k,const T &d){
                SpinlockGuard guard(lock);
                auto & vec=ht[KEY_HASH(k)%N];
                for(Value &v:vec){
                    if(compare(v.k,k)){
                        v.v=d;
                        return;
                    }
                }
                vec.push_back(Value(k,d));
            }
            
            void set(CK k,T && d){
                SpinlockGuard guard(lock);
                auto & vec=ht[KEY_HASH(k)%N];
                for(Value &v:vec){
                    if(compare(v.k,k)){
                        v.v=TMP::move(d);
                        return;
                    }
                }
                vec.push_back(Value(k,TMP::move(d)));
            }
            
            bool unset(CK k){
                auto & vec=ht[KEY_HASH(k)%N];
                size_t i=0;
                const size_t len=vec.size();
                for(i=0;i<len&&!compare(vec[i].k,k);i++);
                if(i==vec.size())return false;
                vec.erase(i);
                return true;
            }
            
            template<typename CB>//void(value)
            void foreach_v(CB && callback){
                for(uint32_t i=0;i<N;i++){
                    for(Value &v:ht[i]){
                        callback(v.v);
                    }
                }
            }
            
            template<typename CB>//void(key)
            void foreach_k(CB && callback){
                for(uint32_t i=0;i<N;i++){
                    for(Value &v:ht[i]){
                        callback(v.k);
                    }
                }
            }
            
            template<typename CB>//void(key,value)
            void foreach_kv(CB && callback){
                for(uint32_t i=0;i<N;i++){
                    for(Value &v:ht[i]){
                        callback(v.k,v.v);
                    }
                }
            }
    };
}


#endif // HASHTABLE_H
