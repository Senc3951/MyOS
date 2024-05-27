#include <arch/lapic.h>
#include <arch/madt.h>
#include <arch/cpu.h>
#include <arch/idt.h>
#include <mem/vmm.h>
#include <log.h>
#include <panic.h>
#include <assert.h>

#define SVR_ENABLE (1 << 8)

static uint64_t g_lapic = 0;

static void hardwareEnable(uintptr_t addr)
{
    #define IA32_APIC_BASE_MSR_ENABLE   (1 << 11)
    #define IA32_APIC_BASE_MSR          0x1B
        
    uint32_t edx = 0, unused;
    uint32_t eax = (addr & 0xfffff0000) | IA32_APIC_BASE_MSR_ENABLE;

#ifdef __PHYSICAL_MEMORY_EXTENSION__
    edx = (addr >> 32) & 0x0f;
#endif
    
    // Enable
    __wrmsr(IA32_APIC_BASE_MSR, eax, edx);
    
    // Verify it was enabled
    __cpuid(1, eax, unused, unused, edx);
    assert(edx & CPU_APIC);
}

static void spuriousInterruptHandler(interrupt_frame_t *frame)
{
    LOG_WARNING_NO_INFO("Spurious interrupt at %u:%p\n", frame->cs, frame->eip);
    lapic_eoi();
}

static void errorInterruptHandler(interrupt_frame_t *frame)
{
    panic("Local APIC error interrupt from %u:%p", frame->cs, frame->eip);
}

void lapic_init()
{
    g_lapic = _MADT.lapic;

    // Hardware enale apic
    hardwareEnable(g_lapic);
    
    // Software enable apic
    register_interrupt(ISR_SPURIOUS, spuriousInterruptHandler);
    lapicw(LAPIC_SVR, SVR_ENABLE | ISR_SPURIOUS);
    
    // Disable logical interrupt lines
    lapicw(LAPIC_LINT0, LAPIC_MASKED);
    lapicw(LAPIC_LINT1, LAPIC_MASKED);
    
    // Disable performance counter overflow interrupts on machine that provide that interrupt entry
    if (((lapicr(LAPIC_VER) >> 16) & 0xFF) >= 4)
        lapicw(LAPIC_PERF, LAPIC_MASKED);

    // Map error interrupt
    register_interrupt(ISR_ERROR, errorInterruptHandler);
    lapicw(LAPIC_ERROR, ISR_ERROR);
    
    // Clear error status register
    lapicw(LAPIC_ESR, 0);
    lapicw(LAPIC_ESR, 0);

    // Acknowledge any interrupt
    lapic_eoi();
    
    // Send an Init level De-Assert to synchronize arbitration id's
    lapicw(LAPIC_ICRHI, 0);
    lapicw(LAPIC_ICRLO, LAPIC_BCAST | LAPIC_INIT | LAPIC_LEVEL);
    while (lapicr(LAPIC_ICRLO) & LAPIC_DELIVS) ;
    
    // Enable interrupts on the APIC
    lapicw(LAPIC_TPR, 0);
    
    LOG_WARNING_INFO("Local APIC version %u at %llx. Running with %u processor(s)\n", (uint8_t)lapicr(LAPIC_VER), g_lapic, _MADT.processorCount);
}

void lapic_eoi()
{
    lapicw(LAPIC_EOI, 0);
}

uint8_t lapic_id()
{
    return lapicr(LAPIC_ID) >> 24;
}

uint32_t lapicr(const uint32_t offset)
{
    return *(volatile uint32_t *)(g_lapic + offset);
}

void lapicw(const uint32_t offset, const uint32_t value)
{
    *(volatile uint32_t *)(g_lapic + offset) = value;
}