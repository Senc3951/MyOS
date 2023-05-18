#pragma once

#include <io/io.h>
#include <kprintf.h>
#include <arch/isr.h>
#include <dev/display/vga.h>

/**
 * @brief Print a panic message and dump the stack.
 * @attention This will halt the system infinitely.
 * 
 * @param stack Stack before the panic.
*/
#define IKPANIC(stack, ...) ({\
    vga_clrcsn(BLUE); \
    \
    ksprintf("\n===== [PANIC] =====\n"); \
    kprintf("[%s:%s:%d]\n", __FILE__, __FUNCTION__, __LINE__); \
    kprintf(__VA_ARGS__); \
    isr_dump_registers(stack); \
    isr_dump_stack_trace(stack, ISR_STACK_TRACE_MAX); \
    \
    CLI_HALT(); \
})

/**
 * @brief Print a panic message.
 * @attention This will halt the system infinitely.
*/
#define KPANIC(...) {\
    vga_clrcsn(BLUE); \
    \
    ksprintf("\n===== [PANIC] =====\n"); \
    kprintf("[%s:%s:%d]\n", __FILE__, __FUNCTION__, __LINE__); \
    kprintf(__VA_ARGS__); \
    \
    CLI_HALT(); \
}
