#pragma once

#include <kernel/io/io.h>
#include <kernel/dev/display/vga.h>

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
