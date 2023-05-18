#pragma once

#include <stdbool.h>
#include <dev/display/kprintf.h>

#define LOG(...) ({ \
    ksprintf(true, "[%s:%s:%d] ", __FILE__, __FUNCTION__, __LINE__); \
    ksprintf(true, __VA_ARGS__); \
})
