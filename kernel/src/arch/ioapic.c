#include <arch/ioapic.h>
#include <arch/madt.h>
#include <arch/idt.h>
#include <log.h>
#include <panic.h>

#define IOREGSEL            0
#define IOWIN               0x10
#define IOREDTBL0           0x10

#define IOAPIC_ID           0
#define IOAPIC_VER          1
#define INTERRUPT_MASKED    (1 << 16)

#define READ_REGISTER(base, offset) ({                      \
    *(volatile uint32_t *)(base + IOREGSEL) = offset;       \
    *(volatile uint32_t *)(base + IOWIN);                   \
})
#define WRITE_REGISTER(base, offset, value) ({              \
    *(volatile uint32_t *)(base + IOREGSEL) = offset;       \
    *(volatile uint32_t *)(base + IOWIN) = value;           \
})
#define MAX_INTERRUPTS(addr) ({                             \
    ((uint8_t)(READ_REGISTER(addr, IOAPIC_VER) >> 16) + 1); \
})

static void mapEntry(const uintptr_t base, uint8_t index, uint64_t val)
{
    WRITE_REGISTER(base, IOREDTBL0 + index * 2, (uint32_t)val);
    WRITE_REGISTER(base, IOREDTBL0 + index * 2 + 1, (uint32_t)(val >> 32));    
}

void ioapic_init()
{
    for (size_t i = 0; i < _MADT.ioapicCount; i++)
    {
        ioapic_t *ioapic = _MADT.ioapic[i];
        uintptr_t ioapicAddress = ioapic->address;
        
        // Verify id
        uint8_t id = READ_REGISTER(ioapicAddress, IOAPIC_ID) >> 24;
        if (id != ioapic->id)
            panic("ioapic_id != id. initialized not from bsp");
        
        // Map all interrupts as edge triggered, active high, disabled and not routed
        size_t maxInterrupts = MAX_INTERRUPTS(ioapicAddress);
        for (size_t i = 0; i < maxInterrupts; i++)
            mapEntry(ioapicAddress, i, (IRQ0 + i) | INTERRUPT_MASKED);
        
        LOG_WARNING_INFO("I/O APIC (%u) version %u at %p. irq available at %u-%u\n", i, (uint8_t)READ_REGISTER(ioapicAddress, IOAPIC_VER), ioapicAddress, ioapic->gsib, ioapic->gsib + maxInterrupts);
    }
}

void ioapic_map_irq(const uint8_t irq, const uint8_t dest)
{
    // Get ioapic by interrupt
    ioapic_t *ioapic = NULL;
    for (size_t i = 0; i < _MADT.ioapicCount; i++)
    {
        ioapic = _MADT.ioapic[i];
        if (irq >= ioapic->gsib && irq <= ioapic->gsib + MAX_INTERRUPTS(ioapic->address))
            break;

        ioapic = NULL;  // Reset
    }
    if (!ioapic)
        panic("failed finding I/O APIC available to map irq %u\n", dest);
    
    uint64_t value = IRQ0 + irq;        // Fixed, Physical, Active high, Edge Triggered, Unmasked
    value |= ((uint64_t)dest << 56);    // Destination
    mapEntry(ioapic->address, irq, value);
}