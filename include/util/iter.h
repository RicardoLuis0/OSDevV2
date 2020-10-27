#ifndef ITER_H_INCLUDED
#define ITER_H_INCLUDED

namespace Util {
    template<typename T>
    class CArrayIter{
        T * _arr;
        size_t _n;
        size_t i;
    public:
        CArrayIter(const CArrayIter &other):_arr(other._arr),_n(other._n){
        }
        
        CArrayIter(T * arr,size_t n):_arr(arr),_n(n){
        }
        
        T * begin(){
            return _arr;
        }
        
        T * end(){
            return _arr+_n;//past-the-end pointer
        }
        
        const T * begin() const{
            return _arr;
        }
        
        const T * end() const{
            return _arr+_n;//past-the-end pointer
        }
    };
}

#endif // ITER_H_INCLUDED
