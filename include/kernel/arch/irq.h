#pragma once

#include "isr.h"
#include <stdbool.h>

#define IRQ_SLAVE   2
#define IRQ0        0x20
#define IRQ15       0x2F

#define IS_IRQ(interrupt) (interrupt >= IRQ0 && interrupt <= IRQ15)

enum irq_interrupts
{
    IRQ_PIT = 0,
    IRQ_KEYBOARD,
    IRQ_CASCADE,
    IRQ_COM2,
    IRQ_COM1,
    IRQ_LPT2,
    IRQ_FLOPPY,
    IRQ_LPT1,
    IRQ_CMOS,
    IRQ_MOUSE = 12,
    IRQ_FPU,
    IRQ_PRIMARY_ATA,
    IRQ_SECONDARY_ATA
};

/**
 * @brief Register and IRQ handler.
 * 
 * @param interruptNumber IRQ pin number.
 * @param handler Handler to be called on the interrupt.
 * @param autoUnmask Unmask the IRQ pin immediately after registering an interrupt.
*/
void irq_register_interrupt(const uint8_t interruptNumber, ISRHandler handler, const bool autoUnmask);

/**
 * @brief IRQ Interrupt handler.
 * 
 * @param stack Stack information before the interrupt.
*/
void irq_interrupt_handler(struct interrupt_stack *stack);