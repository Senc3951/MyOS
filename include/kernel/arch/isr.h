#pragma once

#include <stdint.h>

#define ISR_STACK_TRACE_MAX 20

typedef struct InterruptStack
{
    uint64_t ds;
    uint64_t rax, rbx, rcx, rdx, rsi, rdi, rbp;
    uint64_t cr0, cr2, cr3;
    uint64_t r8, r9, r10, r11, r12, r13, r14, r15;
    uint64_t interruptNumber, errorCode;
    uint64_t rip, cs, rflags, rsp, ss;
} __attribute__((packed)) InterruptStack_t;

typedef struct StackFrame
{
    struct StackFrame *rbp;
    uint64_t rip;
} __attribute__((packed)) StackFrame_t;

typedef void (*ISRHandler)(InterruptStack_t *stack);

/**
 * @brief Initialize the ISR.
*/
void ISR_Initialize();

/**
 * @brief Register an interrupt handler.
 * @warning Do not register an IRQ handler here. Include the IRQ header and register it from there.
 * 
 * @param interruptNumber Interrupt to handle.
 * @param handler Method that will handle the interrupt.
*/
void ISR_RegisterInterrupt(const uint8_t interruptNumber, ISRHandler handler);

/**
 * @brief Handler that is called on each interrupt.
 * 
 * @param Stack Current stack.
*/
void ISR_InterruptHandler(InterruptStack_t *stack);

/**
 * @brief Dump all stack calls.
*/
void ISR_DumpStackTrace(InterruptStack_t *stack, const uint16_t traceSize);

/**
 * @brief Dump the contents of the registers.
 * 
 * @param Stack Stack information before the interrupt.
*/
void ISR_DumpRegisters(InterruptStack_t *stack);