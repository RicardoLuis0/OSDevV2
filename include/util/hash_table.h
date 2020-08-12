#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "util/vector.h"
#include "util/tmp.h"
#include "klib.h"

namespace Util {
    
    //if you change key type, you need to also change the compare,remove,hash,clone functions
    template<typename T,                        //type
             typename K=char *,                 //internal key type
             typename CK=const char *,          //input key type
             auto KEY_COMPARE=k_strcmp_bool,    //key comparsion function, required for pointers, if nullptr will use '=='
             auto KEY_REMOVE=free,              //key removal function, required for pointers
             auto KEY_HASH=k_hash_s,            //key hash function, required
             auto KEY_CLONE=strdup,             //key duplication function, required for pointers, if nullptr will use '='
             size_t L=256                       //size of hash table, higher sized lower collision, but increase memory usage
            >
    class HashTable {//TODO REIMPLEMENT WITH Util::String INSTEAD OF C STRINGS
            Spinlock lock;
            static bool compare(K&k,CK&ck){
                if constexpr(TMP::is_null_pointer_dv<KEY_COMPARE>){
                    return (k==ck);
                }else{
                    return KEY_COMPARE(k,ck);
                }
            }
            static void clone(K&k,CK&ck){
                if constexpr(TMP::is_null_pointer_dv<KEY_CLONE>){
                    k=ck;
                }else{
                    k=KEY_CLONE(ck);
                }
            }
            struct Value {
                Value(CK k2,const T &v2){//copy
                    clone(k,k2);
                    v=v2;
                }
                Value(CK k2,T && v2){//move
                    clone(k,k2);
                    v=TMP::move(v2);
                }
                
                Value(const Value &other){
                    clone(k,other.k);
                    v=other.v;
                }
                
                Value(Value && other){
                    k=TMP::move(other.k);
                    if constexpr(TMP::is_pointer_v<K>){
                        other.k=nullptr;
                    }
                    v=TMP::move(other.v);
                }
                ~Value(){
                    if constexpr(!TMP::is_null_pointer_dv<KEY_REMOVE>){
                        KEY_REMOVE(k);
                    }
                }
                K k;
                T v;
            };
            Util::Vector<Value> ht[L];
        public:
            HashTable(){
                static_assert(!TMP::is_null_pointer_dv<KEY_HASH>,"KEY_HASH must not be nullptr");
            }
            
            bool has(CK k){
                SpinlockGuard guard(lock);
                size_t hash=KEY_HASH(k)%L;
                for(Value &v:ht[hash]){
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
                size_t hash=KEY_HASH(k)%L;
                for(Value &v:ht[hash]){
                    if(compare(v.k,k)){
                        return v.v;
                    }
                }
                ht[hash].push(Value(k,T()));
                return ht[hash].back().v;
            }
            
            T& at_else(CK k,T&&t){
                SpinlockGuard guard(lock);
                size_t hash=KEY_HASH(k)%L;
                for(Value &v:ht[hash]){
                    if(compare(v.k,k)){
                        return v.v;
                    }
                }
                return t;
            }
            
            T * find(CK k){//if found return pointer, else null
                SpinlockGuard guard(lock);
                size_t hash=KEY_HASH(k)%L;
                for(Value &v:ht[hash]){
                    if(compare(v.k,k)){
                        return &v.v;
                    }
                }
                return nullptr;
            }
            
            void set(CK k,const T &d){
                SpinlockGuard guard(lock);
                size_t hash=KEY_HASH(k)%L;
                for(Value &v:ht[hash]){
                    if(compare(v.k,k)){
                        v.v=d;
                        return;
                    }
                }
                ht[hash].push(Value(k,d));
            }
            
            void set(CK k,T && d){
                SpinlockGuard guard(lock);
                size_t hash=KEY_HASH(k)%L;
                for(Value &v:ht[hash]){
                    if(compare(v.k,k)){
                        v.v=TMP::move(d);
                        return;
                    }
                }
                ht[hash].push(Value(k,TMP::move(d)));
            }
            
            bool unset(CK k){
                size_t hash=KEY_HASH(k)%L;
                size_t i=0;
                auto & vec=ht[hash];
                for(Value &v:vec){
                    if(compare(v.k,k)){
                        break;
                    }
                    i++;
                }
                if(i==vec.size())return false;
                vec.erase(i);
                return true;
            }
            
            template<typename CB>//void(value)
            void foreach_v(CB && callback){
                for(uint32_t i=0;i<L;i++){
                    for(Value &v:ht[i]){
                        callback(v.v);
                    }
                }
            }
            
            template<typename CB>//void(key)
            void foreach_k(CB && callback){
                for(uint32_t i=0;i<L;i++){
                    for(Value &v:ht[i]){
                        callback(v.k);
                    }
                }
            }
            
            template<typename CB>//void(key,value)
            void foreach_kv(CB && callback){
                for(uint32_t i=0;i<L;i++){
                    for(Value &v:ht[i]){
                        callback(v.k,v.v);
                    }
                }
            }
    };
}


#endif // HASHTABLE_H
