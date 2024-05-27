#pragma once

#include <common.h>

enum
{
    IDT_FLAGS_GATE_32_INTERRUPT     = 0b1110 << 0,
    IDT_FLAGS_GATE_32_TRAP          = 0b1111 << 0,
    IDT_FLAGS_DPL0                  = 0 << 5,
    IDT_FLAGS_DPL3                  = 3 << 5,
    IDT_FLAGS_PRESENT               = 1 << 7
};

#define IDT_ENTRIES         256
#define IRQ0                0x20
#define IRQ15               0x30
#define ISR2IRQ(isr)        (isr - IRQ0)

#define IDT_INTERRUPT_DPL0  IDT_FLAGS_GATE_32_INTERRUPT | IDT_FLAGS_DPL0 | IDT_FLAGS_PRESENT
#define IDT_INTERRUPT_DPL3  IDT_FLAGS_GATE_32_INTERRUPT | IDT_FLAGS_DPL3 | IDT_FLAGS_PRESENT
#define IDT_TRAP_DPL0       IDT_FLAGS_GATE_32_TRAP | IDT_FLAGS_DPL0 | IDT_FLAGS_PRESENT
#define IDT_TRAP_DPL3       IDT_FLAGS_GATE_32_TRAP | IDT_FLAGS_DPL3 | IDT_FLAGS_PRESENT

// Known interrupts
#define ISR_TIMER       (IRQ0)
#define ISR_KBD         (IRQ0 + 1)
#define ISR_SPURIOUS    (IRQ0 + 31)
#define ISR_ERROR       (IRQ0 + 19)

typedef struct IDT_ENTRY
{
    uint16_t offsetLow;
    uint16_t segment;
    uint8_t reserved;
    uint8_t flags;
    uint16_t offsetHigh;
} __PACKED__ idt_entry_t;

typedef struct INTERRUPT_STACK
{
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    
    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;
    
    uint32_t num;
    uint32_t errorcode;
    
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t useresp;
    uint32_t ss;
} __PACKED__ interrupt_frame_t;

typedef void (*interrupt_handler_t)(interrupt_frame_t *);

void idt_init();
void register_interrupt(const uint8_t num, interrupt_handler_t handler);