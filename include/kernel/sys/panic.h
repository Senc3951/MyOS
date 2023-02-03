#pragma once

#include <kernel/io/io.h>
#include <kernel/arch/isr.h>
#include <kernel/dev/display/vga.h>

/**
 * @brief Print a panic message and dump the stack.
 * @attention This will halt the system infinitely.
 * 
 * @param stack Stack before the panic.
 * @param fmt Formatted string to print.
*/
#define IKPANIC(stack, ...) ({\
    clrscnc(BLUE); \
    kprintf(__VA_ARGS__); \
    ISR_DumpRegisters(stack); \
    ISR_DumpStackTrace(stack, ISR_STACK_TRACE_MAX); \
    \
    __CLI_HALT(); \
})

/**
 * @brief Print a panic message.
 * @attention This will halt the system infinitely.
 * 
 * @param fmt Formatted string to print.
*/
#define KPANIC(...) {\
    clrscnc(BLUE); \
    kprintf("[%s:%s:%d]\n", __FILE__, __FUNCTION__, __LINE__); \
    kprintf(__VA_ARGS__); \
    \
    __CLI_HALT(); \
}
