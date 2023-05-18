#include <arch/idt.h>
#include <arch/gdt.h>
#include <lib/string.h>

extern uint64_t interrupt_handlers[];
static struct idt_entry g_IDTGates[IDT_GATES];

#define IDT_LOAD(idt) ({ \
    asm volatile("lidtq %0" : : "m"(*idt)); \
})

void idt_init()
{
    struct idt idt = 
    {
        .limit = sizeof(g_IDTGates) - 1,
        .base = g_IDTGates
    };
    memset(idt.base, 0, idt.limit);
    
    for (size_t i = 0; i <= 0x2F; i++)
        idt_set_entry(i, interrupt_handlers[i], KERNEL_CS, IDT_INTERRUPT_GATE);
    
    IDT_LOAD(&idt);
}

void idt_set_entry(const uint8_t gateNumber, const uint64_t offset, const uint16_t segment, const uint8_t flags)
{
    struct idt_entry *entry = &g_IDTGates[gateNumber];
    
    entry->offsetLow = (uint16_t)(offset & 0xFFFF);
    entry->offsetMid = (uint16_t)((offset >> 16) & 0xFFFF);
    entry->offsetHigh = (uint32_t)((offset >> 32) & 0xFFFFFFFF);
    
    entry->segment = segment;
    entry->flags = flags;
    entry->empty = 0;
    entry->reserved = 0;
}