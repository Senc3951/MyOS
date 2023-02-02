#include <kernel/arch/idt.h>
#include <kernel/arch/irq.h>
#include <kernel/arch/gdt.h>
#include <kernel/lib/string.h>

extern uint64_t interruptHandlers[];

static IDTEntry_t g_IDTGates[IDT_GATES];

void IDT_Initialize()
{
    InterruptDescriptorTable_t idt = 
    {
        .limit = sizeof(g_IDTGates) - 1,
        .base = g_IDTGates
    };
    memset(idt.base, 0, idt.limit);
    
    for (size_t i = 0; i <= IRQ15; i++)
        IDT_SetGate(i, interruptHandlers[i], KERNEL_CS, IDT_INTERRUPT_GATE);
    
    IDT_Load(&idt);
}

void IDT_Load(const InterruptDescriptorTable_t *idt)
{
    asm volatile("lidtq %0" : : "m"(*idt));
}

void IDT_SetGate(const uint8_t gateNumber, const uint64_t offset, const uint16_t segment, const uint8_t flags)
{
    IDTEntry_t *entry = &g_IDTGates[gateNumber];
    
    entry->offsetLow = (uint16_t)(offset & 0xFFFF);
    entry->offsetMid = (uint16_t)((offset >> 16) & 0xFFFF);
    entry->offsetHigh = (uint32_t)((offset >> 32) & 0xFFFFFFFF);
    
    entry->segment = segment;
    entry->flags = flags;
    entry->empty = 0;
    entry->reserved = 0;
}