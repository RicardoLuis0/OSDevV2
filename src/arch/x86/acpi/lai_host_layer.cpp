#include "arch/x86.h"
#include <stdlib.h>
#include "acpi.h"
#include "util.h"
#include "klib.h"
#include <stdio.h>
#include "lai/host.h"
#include "acpispec/tables.h"


extern acpi_rsdp_t * rsdp;
extern acpi_rsdt_t * rsdt;

extern acpi_xsdp_t * xsdp;
extern acpi_xsdt_t * xsdt;

//#define LAI_HOST_IDENTITY_MAP

extern "C" {
    
    void * laihost_malloc(size_t n){
#if defined(DEBUG) && defined(K_LAI_DEBUG_EXTRA)
        void * p=malloc(n);
        char buf[80];
        snprintf(buf,79,"laihost_malloc(%u)=0x%x",n,(uint32_t)p);
        laihost_log(LAI_DEBUG_LOG,buf);
        return p;
#else
        return malloc(n);
#endif // K_LAI_DEBUG_EXTRA
    }
    
    void laihost_log(int level, const char * msg) {
        switch(level){
        case LAI_DEBUG_LOG:
            #if defined(DEBUG) && defined(K_LAI_DEBUG_EXTRA)
            k_logs("\n[LAI/DEBUG] ");
            k_logs(msg);
            #endif // K_LAI_DEBUG_EXTRA
            break;
        case LAI_WARN_LOG:
            k_puts("\n[LAI/WARN] ");
            k_puts(msg);
            break;
        default:
            k_puts("\n[LAI/UNKNOWN] ");
            k_puts(msg);
        }
    }

    void laihost_panic(const char * msg){
        k_abort_s(msg);
    }

    void * laihost_realloc(void * p, size_t n){
#if defined(DEBUG) && defined(K_LAI_DEBUG_EXTRA)
        void * p2=realloc(p,n);
        char buf[80];
        snprintf(buf,79,"laihost_realloc(0x%x,%u)=0x%x",(uint32_t)p,n,(uint32_t)p2);
        laihost_log(LAI_DEBUG_LOG,buf);
        return p2;
#else
        return realloc(p,n);
#endif // K_LAI_DEBUG_EXTRA
    }

    void laihost_free(void * p){
#if defined(DEBUG) && defined(K_LAI_DEBUG_EXTRA)
        char buf[80];
        snprintf(buf,79,"laihost_free(0x%x)",(uint32_t)p);
        laihost_log(LAI_DEBUG_LOG,buf);
#endif // K_LAI_DEBUG_EXTRA
        free(p);
    }

    void * laihost_map(size_t address, size_t count){
        if(address<4096){
            Memory::Internal::map_null();
        }else if(address>=1_MB){
            uint32_t p_id,n;
            Memory::Internal::pages_for(address,count,p_id,n);
#ifdef LAI_HOST_IDENTITY_MAP
            Memory::Internal::map_virtual_page_unsafe(p_id,p_id,n,true);
#else
    #if defined(DEBUG) && defined(K_LAI_DEBUG_EXTRA)
            void *p = (void*)((Memory::Internal::map_virtual_page_unsafe(p_id,Memory::next_free_virt_page(n),n,false)<<12)+(address%4096));
            char buf[80];
            snprintf(buf,79,"laihost_map(0x%x,%u)=0x%x",(uint32_t)address,count,(uint32_t)p);
            laihost_log(LAI_DEBUG_LOG,buf);
            return p;
    #else
            return (void*)((Memory::Internal::map_virtual_page_unsafe(p_id,Memory::next_free_virt_page(n),n,false)<<12)+(address%4096));
    #endif // K_LAI_DEBUG_EXTRA
#endif // LAI_HOST_IDENTITY_MAP
        }else{
            //lower memory except first page is always mapped, do nothing
        }
#if defined(DEBUG) && defined(K_LAI_DEBUG_EXTRA)
        char buf[80];
        snprintf(buf,79,"laihost_map(0x%x,%u)=0x%x",(uint32_t)address,count,(uint32_t)address);
        laihost_log(LAI_DEBUG_LOG,buf);
#endif // K_LAI_DEBUG_EXTRA
        return (void*)address;
    }

    void * laihost_scan(const char * sig,size_t index){
#if defined(DEBUG) && defined(K_LAI_DEBUG_EXTRA)
        size_t orig_index=index;
#endif // K_LAI_DEBUG_EXTRA
        if(memcmp("DSDT",sig,4)==0){
            acpi_fadt_t * fadt=(acpi_fadt_t *)laihost_scan("FACP",0);
            if(!fadt)k_abort_s("Couldn't find FADT header!");
            if(xsdt){
                if(fadt->x_dsdt){
                    if(fadt->x_dsdt<4_GB){
                        return ACPI::map_table(fadt->x_dsdt);
                    }else{
                        k_abort_s("X_DSDT too far!");
                    }
                }
            }
#if defined(DEBUG) && defined(K_LAI_DEBUG_EXTRA)
            void * t=ACPI::map_table(fadt->dsdt);
            char buf[80];
            snprintf(buf,79,"laihost_scan(%.4s,%u)=0x%x",sig,orig_index,(uint32_t)t);
            laihost_log(LAI_DEBUG_LOG,buf);
            return t;
#else
            return ACPI::map_table(fadt->dsdt);
#endif // K_LAI_DEBUG_EXTRA
        }
        if(xsdt){
            const uint32_t count=((xsdt->header.length-sizeof(acpi_header_t))/sizeof(uint64_t));
            for(uint32_t i=0;i<count;i++){
                void * t=ACPI::map_table(xsdt->tables[i]);
                if(memcmp(t,sig,4)==0){
                    if(index==0){
#if defined(DEBUG) && defined(K_LAI_DEBUG_EXTRA)
                        char buf[80];
                        snprintf(buf,79,"laihost_scan(%.4s,%u)=0x%x",sig,orig_index,(uint32_t)t);
                        laihost_log(LAI_DEBUG_LOG,buf);
#endif // K_LAI_DEBUG_EXTRA
                        return t;
                    }
                    index--;
                }
                ACPI::unmap_table(t);
            }
        }else{
            const uint32_t count=((rsdt->header.length-sizeof(acpi_header_t))/sizeof(uint32_t));
            for(uint32_t i=0;i<count;i++){
                void * t=ACPI::map_table(rsdt->tables[i]);
                if(memcmp(t,sig,4)==0){
                    if(index==0){
#if defined(DEBUG) && defined(K_LAI_DEBUG_EXTRA)
                        char buf[80];
                        snprintf(buf,79,"laihost_scan(%.4s,%u)=0x%x",sig,orig_index,(uint32_t)t);
                        laihost_log(LAI_DEBUG_LOG,buf);
#endif // DEBUG
                        return t;
                    }
                    index--;
                }
                ACPI::unmap_table(t);
            }
        }
#if defined(DEBUG) && defined(K_LAI_DEBUG_EXTRA)
        char buf[80];
        snprintf(buf,79,"laihost_scan(%.4s,%u) not found",sig,orig_index);
        laihost_log(LAI_DEBUG_LOG,buf);
#endif // K_LAI_DEBUG_EXTRA
        return nullptr;//couldn't find header
    }

    void laihost_unmap(void *pointer, size_t count){
#if defined(DEBUG) && defined(K_LAI_DEBUG_EXTRA)
        char buf[80];
        snprintf(buf,79,"laihost_unmap(0x%x,%u)",(uint32_t)pointer,count);
        laihost_log(LAI_DEBUG_LOG,buf);
#endif // K_LAI_DEBUG_EXTRA
        if((uint32_t)pointer<4096){
            Memory::Internal::unmap_null();
        }else if((uint32_t)pointer>=1_MB){
#ifdef LAI_HOST_IDENTITY_MAP
            //if identity mapping don't unmap since a page could have been mapped multiple times
#else
            uint32_t p_id,n;
            Memory::Internal::pages_for((uint32_t)pointer,count,p_id,n);
            Memory::unmap_virtual_page(p_id,n);
#endif // LAI_HOST_IDENTITY_MAP
        }else{
            //lower memory except first page is always mapped, do nothing
        }
    }

    void laihost_outb(uint16_t port, uint8_t val){
        outb(port,val);
    }

    void laihost_outw(uint16_t port, uint16_t val){
        outw(port,val);
    }

    void laihost_outd(uint16_t port, uint32_t val){
        outl(port,val);
    }

    uint8_t laihost_inb(uint16_t port){
        return inb(port);
    }

    uint16_t laihost_inw(uint16_t port){
        return inw(port);
    }

    uint32_t laihost_ind(uint16_t port){
        return inl(port);
    }

    void laihost_sleep(uint64_t ms) {
        k_sleep(ms);
    }

}
