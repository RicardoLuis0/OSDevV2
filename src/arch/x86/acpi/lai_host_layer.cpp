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

extern acpi_fadt_t * fadt;

//#define LAI_HOST_IDENTITY_MAP

extern "C" {
    
    void * laihost_malloc(size_t n){
        return malloc(n);
    }
    
    void laihost_log(int level, const char * msg) {
        switch(level){
        case LAI_DEBUG_LOG:
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
        return realloc(p,n);
    }

    void laihost_free(void * p){
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
            return (void*)((Memory::Internal::map_virtual_page_unsafe(p_id,Memory::next_free_virt_page(n),n,false)<<12)+(address%4096));
#endif // LAI_HOST_IDENTITY_MAP
        }else{
            //lower memory except first page is always mapped, do nothing
        }
        return (void*)address;
    }

    void * laihost_scan(const char * sig,size_t index){
        if(memcmp("DSDT",sig,4)==0){
            if(xsdt){
                if(fadt->x_dsdt){
                    if(fadt->x_dsdt<4_GB){
                        return ACPI::Internal::map_table(fadt->x_dsdt);
                    }else{
                        k_abort_s("X_DSDT too far!");
                    }
                }
            }
            return ACPI::Internal::map_table(fadt->dsdt);
        }
        if(xsdt){
            const uint32_t count=((xsdt->header.length-sizeof(acpi_header_t))/sizeof(uint64_t));
            for(uint32_t i=0;i<count;i++){
                void * t=ACPI::Internal::map_table(xsdt->tables[i]);
                if(memcmp(t,sig,4)==0){
                    if(index==0) return t;
                    index--;
                }
                ACPI::Internal::unmap_table(t);
            }
        }else{
            const uint32_t count=((rsdt->header.length-sizeof(acpi_header_t))/sizeof(uint32_t));
            for(uint32_t i=0;i<count;i++){
                void * t=ACPI::Internal::map_table(rsdt->tables[i]);
                if(memcmp(t,sig,4)==0){
                    if(index==0){
                        return t;
                    }
                    index--;
                }
                ACPI::Internal::unmap_table(t);
            }
        }
        return nullptr;//couldn't find header
    }

    void laihost_unmap(void *pointer, size_t count){
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

    void laihost_pci_writeb(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, uint16_t offset, uint8_t val){
        PCI::writeb(bus,slot,fun,offset,val);
    }

    void laihost_pci_writew(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, uint16_t offset, uint16_t val){
        PCI::writew(bus,slot,fun,offset,val);
    }

    void laihost_pci_writed(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, uint16_t offset, uint32_t val){
        PCI::writed(bus,slot,fun,offset,val);
    }

    uint8_t laihost_pci_readb(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, uint16_t offset){
        return PCI::readb(bus,slot,fun,offset);
    }

    uint16_t laihost_pci_readw(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, uint16_t offset){
        return PCI::readw(bus,slot,fun,offset);
    }

    uint32_t laihost_pci_readd(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, uint16_t offset){
        return PCI::readd(bus,slot,fun,offset);
    }

}
