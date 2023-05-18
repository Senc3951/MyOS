#pragma once

#include <stdint.h>

#define ISR_STACK_TRACE_MAX 20

struct interrupt_stack
{
    uint64_t ds;
    uint64_t rax, rbx, rcx, rdx, rsi, rdi, rbp;
    uint64_t cr0, cr2, cr3;
    uint64_t r8, r9, r10, r11, r12, r13, r14, r15;
    uint64_t interruptNumber, errorCode;
    uint64_t rip, cs, rflags, rsp, ss;
} __attribute__((packed));

struct stack_frame
{
    struct stack_frame *rbp;
    uint64_t rip;
} __attribute__((packed));

typedef void (*ISRHandler)(struct interrupt_stack *stack);

/**
 * @brief Register an interrupt handler.
 * 
 * @param interruptNumber Interrupt to handle.
 * @param handler Method to be called on the interrupt.
*/
void isr_register_interrupt(const uint8_t interruptNumber, ISRHandler handler);

/**
 * @brief Handler that is called on each interrupt.
 * 
 * @param stack Current stack.
*/
void isr_interrupt_handler(struct interrupt_stack *stack);

/**
 * @brief Dump the contents of the registers.
 * 
 * @param stack Stack information before the interrupt.
*/
void isr_dump_registers(struct interrupt_stack *stack);

/**
 * @brief Dump all stack calls.
 * 
 * @param stack Stack information before the interrupt.
 * @param maxTraceSize Max number of functions to trace back to.
*/
void isr_dump_stack_trace(struct interrupt_stack *stack, const uint16_t maxTraceSize);