#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "util/vector.h"
#include "util/tmp.h"
#include "klib.h"

namespace Util {
    template<typename T,typename U,auto KC> struct COMPARE {
        static bool compare(T&t,U&u){
            return KC(t,u);
        }
    };
    template<typename T,typename U> struct COMPARE<T,U,nullptr> {
        static bool remove(T&t,U&u){
            return t==u;
        }
    };
    template<typename T,typename U,auto CV> struct CLONE {
        static void clone(T&t,U&u){
            t=CV(u);
        }
    };
    template<typename T,typename U> struct CLONE<T,U,nullptr> {
        static void clone(T&t,U&u){
            t=u;
        }
    };
    //if you change key type, you need to also change the compare,remove,hash,clone functions
    template<typename T,                        //type
             typename K=char *,                 //internal key type
             typename CK=const char *,          //input key type
             auto KEY_COMPARE=k_strcmp_bool,    //key comparsion function, required for pointers, if null will use '=='
             auto KEY_REMOVE=free,              //key removal function, required for pointers, if null will use destructor
             auto KEY_HASH=k_hash_s,            //key hash function, required
             auto KEY_CLONE=strdup,             //key duplication function, required for pointers, if null will use '='
             size_t L=256                       //size of hash table, higher sized lower collision, but increase memory usage
            >
    class HashTable {//TODO REIMPLEMENT WITH Util::String INSTEAD OF C STRINGS
            Spinlock lock;
            struct Value {
                Value(CK k2,const T &v2){//copy
                    CLONE<K,CK,KEY_CLONE>::clone(k,k2);
                    v=v2;
                }
                Value(CK k2,T && v2){//move
                    CLONE<K,CK,KEY_CLONE>::clone(k,k2);
                    v=TMP::move(v2);
                }
                static void VALUE_REMOVE(Value &v){
                    REMOVE<K,KEY_REMOVE>::remove(v.k);
                }
                K k;
                T v;
            };
            Util::Vector<Value,Value::VALUE_REMOVE> ht[L];
        public:
            HashTable(){
                static_assert(!TMP::is_null_pointer_v<decltype(KEY_HASH)>,"KEY_HASH must not be nullptr");
            }
            
            bool has(CK k){
                SpinlockGuard guard(lock);
                size_t hash=KEY_HASH(k)%L;
                for(Value &v:ht[hash]){
                    if(COMPARE<K,CK,KEY_COMPARE>::compare(v.k,k)){
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
                    if(COMPARE<K,CK,KEY_COMPARE>::compare(v.k,k)){
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
                    if(COMPARE<K,CK,KEY_COMPARE>::compare(v.k,k)){
                        return v.v;
                    }
                }
                return t;
            }
            
            T * find(CK k){//if found return pointer, else null
                SpinlockGuard guard(lock);
                size_t hash=KEY_HASH(k)%L;
                for(Value &v:ht[hash]){
                    if(COMPARE<K,CK,KEY_COMPARE>::compare(v.k,k)){
                        return &v.v;
                    }
                }
                return nullptr;
            }
            
            void set(CK k,const T &d){
                SpinlockGuard guard(lock);
                size_t hash=KEY_HASH(k)%L;
                for(Value &v:ht[hash]){
                    if(COMPARE<K,CK,KEY_COMPARE>::compare(v.k,k)){
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
                    if(COMPARE<K,CK,KEY_COMPARE>::compare(v.k,k)){
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
                    if(COMPARE<K,CK,KEY_COMPARE>::compare(v.k,k)){
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
