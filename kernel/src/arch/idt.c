#include <arch/idt.h>
#include <arch/cpu.h>
#include <arch/lapic.h>
#include <panic.h>
#include <log.h>

static __ALIGNED__(sizeof(idt_entry_t)) idt_entry_t g_entries[IDT_ENTRIES];
static __ALIGNED__(sizeof(uintptr_t)) table_descriptor_t g_descriptor;
static interrupt_handler_t g_handlers[IDT_ENTRIES];

extern uintptr_t _interrupt_handlers[IDT_ENTRIES];

static inline void setIDTEntry(const uint8_t num, uintptr_t handler, const uint8_t flags)
{
    idt_entry_t *entry = &g_entries[num];
    
    entry->offsetLow = (uint16_t)(handler & 0xFFFF);
    entry->segment = KERNEL_CODE_SEGMENT;
    entry->reserved = 0;
    entry->flags = flags;
    entry->offsetHigh = (uint16_t)((handler >> 16) & 0xFFFF);
}

void idt_init()
{
    // Populate idt
    for (uint16_t i = 0; i < IDT_ENTRIES; i++)
        setIDTEntry(i, _interrupt_handlers[i], IDT_INTERRUPT_DPL0);
    
    // Load idt
    g_descriptor.size = (uint16_t)(sizeof(g_entries) - 1);
    g_descriptor.offset = (uint32_t)g_entries;
    uintptr_t idtAddress = (uintptr_t)&g_descriptor;
    load_idt(idtAddress);
    
    LOG("IDT loaded at %p\n", idtAddress);
}

void _hl_interrupt_handler(interrupt_frame_t *stack)
{
    const uint32_t num = stack->num;
    if (!g_handlers[num])
        panic("interrupt 0x%X does not have a handler", num);
    
    // Call handler
    g_handlers[num](stack);
    
    // Send eoi
    if (num >= IRQ0 && num <= IRQ15)
        lapic_eoi();
}

void register_interrupt(const uint8_t num, interrupt_handler_t handler)
{
    if (g_handlers[num])
        LOG_ERROR_INFO("Overwriting handler for interrupt 0x%X\n", num);
    
    g_handlers[num] = handler;
    LOG("Registered a handler at %p for interrupt 0x%X\n", (uintptr_t)handler, num);
}