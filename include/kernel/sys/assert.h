#pragma once

#include <io/io.h>
#include <io/vga.h>
#include <kprintf.h>

#define ASSERT(condition, ...) ({\
    if ((int)condition) {\
    } else {\
        vga_clrcsn(BLUE); \
        kprintf("[%s:%s:%d]\nAssertation Failed. ", __FILE__, __FUNCTION__, __LINE__); \
        kprintf(__VA_ARGS__); \
        \
        CLI_HALT(); \
    } \
})
