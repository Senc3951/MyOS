#include <arch/madt.h>
#include <arch/rsdt.h>
#include <arch/cpu.h>
#include <mem/vmm.h>

typedef struct MADT_TABLE
{
    rsdt_header_t header;
    uint32_t lapicAddress;
    uint32_t flags;
} __PACKED__ madt_table_t;

#define PROCESSOR_LAPIC     0
#define IO_APIC             1
#define IO_APIC_OVERRIDE    2
#define LAPIC_OVERRIDE      5

madt_t _MADT = { 0 };

void madt_init()
{
    madt_table_t *madt = (madt_table_t *)rsdt_find_table(RSDT_NAME_APIC);
    _MADT.lapic = madt->lapicAddress;
    
    uint8_t *end = (uint8_t *)madt + madt->header.length;
    uint8_t *ptr = (uint8_t *)madt;
    for (ptr += 44; ptr < end; ptr += ptr[1])
    {
        switch (ptr[0])
        {
            case PROCESSOR_LAPIC:
                if (ptr[4] & 3)
                    _MADT.processorID[_MADT.processorCount++] = ptr[3];
                
                break;
            case IO_APIC:
                ioapic_t *_ioapic = (ioapic_t *)(ptr + 2);
                _MADT.ioapic[_MADT.ioapicCount++] = _ioapic;
                
                // Map ioapic
                uintptr_t ioapicRoundedAddress = rndown(_ioapic->address, PAGE_SIZE);
                pt_imap(get_kernel_pd(), ioapicRoundedAddress, PAGING_READ_WRITE);
                __flush_tlb(ioapicRoundedAddress);
                
                break;
            case LAPIC_OVERRIDE:
                _MADT.lapic = (uint64_t)(ptr + 4);
                break;
        }
    }
    
    // Map Local APIC
    uint64_t lapicRoundedAddress = rndown(_MADT.lapic, PAGE_SIZE);
    pt_imap(get_kernel_pd(), lapicRoundedAddress, PAGING_READ_WRITE);
    __flush_tlb(lapicRoundedAddress);
}