#pragma once

#include "isr.h"
#include <stdbool.h>

#define IRQ_SLAVE   2
#define IRQ0        0x20
#define IRQ15       0x2F

#define IS_IRQ(interrupt) (interrupt >= IRQ0 && interrupt <= IRQ15)

/**
 * @brief Register and IRQ handler.
 * 
 * @param interruptNumber IRQ pin number.
 * @param handler Handler to be called on the interrupt.
 * @param autoUnmask Unmask the IRQ pin immediately after registering an interrupt
*/
void IRQ_RegisterInterrupt(const uint8_t interruptNumber, ISRHandler handler, const bool autoUnmask);

/**
 * @brief IRQ Interrupt handler.
 * 
 * @param stack Stack information before the interrupt.
*/
void IRQ_InterruptHandler(InterruptStack_t *stack);