#include "arch/x86.h"
#include <stdlib.h>
#include "util.h"

extern "C" {

    void * laihost_malloc(size_t n){
        return malloc(n);
    }

    void * laihost_realloc(void * p, size_t n){
        return realloc(p,n);
    }

    void laihost_free(void * p){
        free(p);
    }

    void * laihost_map(size_t address, size_t count){
        if(address<4096){
            Memory::Internal::map_null();
            return (void*)address;
        }else if(address>=1_MB){
            uint32_t p_id,n,offset=address%4096;
            Memory::Internal::pages_for(address,count,p_id,n);
            Memory::map_virtual_page(address>>12,p_id,n);
            return (void*)((p_id>>12)+offset);
        }else{
            //lower memory except first page is always mapped, do nothing
            return (void*)address;
        }
    }

    void laihost_unmap(void *pointer, size_t count){
        if((uint32_t)pointer<4096){
            Memory::Internal::unmap_null();
        }else if((uint32_t)pointer>=1_MB){
            uint32_t p_id,n;
            Memory::Internal::pages_for((uint32_t)pointer,count,p_id,n);
            Memory::unmap_virtual_page(p_id,n);
        }else{
            //lower memory except first page is always mapped, do nothing
        }
    }

}
