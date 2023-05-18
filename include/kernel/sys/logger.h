#pragma once

#include <dev/display/kprintf.h>

#define LOG(...) ({ \
    ksprintf("[%s:%s:%d] ", __FILE__, __FUNCTION__, __LINE__); \
    ksprintf(__VA_ARGS__); \
})
