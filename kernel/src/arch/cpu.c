#include <arch/cpu.h>
#include <log.h>

extern void x32_load_gdt(uintptr_t addr, const uint8_t cs, const uint8_t ds);
extern void x32_load_idt(uintptr_t addr);

void load_gdt(const uintptr_t addr)
{
    x32_load_gdt(addr, KERNEL_CODE_SEGMENT, KERNEL_DATA_SEGMENT);
}

void load_tss(const uint16_t offset)
{
    asm volatile("ltr %0" : : "r"(offset) : );
}

void load_idt(const uintptr_t addr)
{
    x32_load_idt(addr);
}

void __flush_tlb(uintptr_t addr)
{
    asm volatile("invlpg (%0)" ::"r" (addr) : "memory");
}

uint32_t __xchg(volatile uint32_t *addr, uint32_t val)
{
    uint32_t result;

    // The + in "+m" denotes a read-modify-write operand.
    asm volatile("lock; xchgl %0, %1" :
            "+m" (*addr), "=a" (result) :
            "1" (val) :
            "cc");
    
    return result;
}

uintptr_t __readcr2()
{
    uintptr_t val;
    asm volatile("movl %%cr2, %0" : "=r" (val));
    
    return val;
}

uint32_t __readflags()
{
    uint32_t eflags;
    asm volatile("pushfl; popl %0" : "=r" (eflags));
    
    return eflags;
}

void __rdmsr(uint32_t msr, uint32_t *low, uint32_t *high)
{
    asm volatile("rdmsr" : "=a"(*low), "=d"(*high) : "c"(msr));
}

void __wrmsr(uint32_t msr, uint32_t low, uint32_t high)
{
    asm volatile("wrmsr" : : "a"(low), "d"(high), "c"(msr));
}