#pragma once

#include <kprintf.h>

/**
 * @brief Print a regular log message.
*/
#define LOG(...) ({ \
    ksprintf("[%s:%s:%d] ", __FILE__, __FUNCTION__, __LINE__); \
    ksprintf(__VA_ARGS__); \
})

/**
 * @brief Print an important log message.
*/
#define LOGI(...) ({ \
    ksprintf("[%s:%s:%d] [", __FILE__, __FUNCTION__, __LINE__); \
    ksprintf(__VA_ARGS__); \
    ksprintf("]\n"); \
})

/**
 * @brief Print a log message without the file, function, line.
*/
#define LOGN(...) ({ \
    ksprintf(__VA_ARGS__); \
})
