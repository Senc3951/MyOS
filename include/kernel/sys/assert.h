#pragma once

#include <kernel/io/io.h>
#include <kernel/dev/display/vga.h>

#define ASSERT(condition, ...) ({\
    if ((int)condition) {\
    } else {\
        clrscnc(BLUE); \
        kprintf("[%s:%s:%d]\nAssertation Failed. ", __FILE__, __FUNCTION__, __LINE__); \
        kprintf(__VA_ARGS__); \
        \
        __CLI_HALT(); \
    } \
})
