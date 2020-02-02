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
             auto KEY_REMOVE=k_free,            //key removal function, required for pointers, if null will use destructor
             auto KEY_HASH=k_hash_s,            //key hash function, required
             auto KEY_CLONE=k_strdup,           //key duplication function, required for pointers, if null will use '='
             size_t L=256                       //size of hash table, higher sized lower collision, but increase memory usage
            >
    class HashTable {
            Spinlock lock;
            struct Value {
                Value(CK k2,T v2){
                    CLONE<K,CK,KEY_CLONE>::clone(k,k2);
                    v=v2;
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

            void set(CK k,T d){
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

            void foreach(void(*callback)(T&)){
                for(uint32_t i=0;i<L;i++){
                    for(Value &v:ht[i]){
                        callback(v.v);
                    }
                }
            }
    };
}


#endif // HASHTABLE_H
