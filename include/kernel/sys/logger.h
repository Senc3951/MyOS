#pragma once

#include <kernel/dev/display/vga.h>

#define DEBUG

#ifdef DEBUG
    #define LOG_INFO(module, b, f, ...) ({ \
        setscnc(b, f); \
        if (module != NULL) \
            kprintf("[%s] ", module); \
        \
        kprintf(__VA_ARGS__); \
        setscnc(BLACK, WHITE); \
    })

    #define LOGI(module, ...) (LOG_INFO(module, BLACK, WHITE, __VA_ARGS__))
    #define LOGW(module, ...) (LOG_INFO(module, BLACK, BROWN, __VA_ARGS__))
    #define LOGE(module, ...) (LOG_INFO(module, BLACK, RED, __VA_ARGS__))
    #define LOGCE(module, ...) (LOG_INFO(module, RED, WHITE, __VA_ARGS__))
#else
    #define LOGI(module, ...) ( )
    #define LOGW(module, ...) ( )
    #define LOGC(module, ...) ( )
    #define LOGCE(module, ...) ( )    
#endif