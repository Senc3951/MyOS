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
    
    #define LOGINM(...) (LOG_INFO(NULL, BLACK, WHITE, __VA_ARGS__))
    #define LOGWNM(...) (LOG_INFO(NULL, BLACK, BROWN, __VA_ARGS__))
    #define LOGENM(...) (LOG_INFO(NULL, BLACK, RED, __VA_ARGS__))
    #define LOGCDNM(...) (LOG_INFO(NULL, RED, WHITE, __VA_ARGS__))

    #define LOGI(module, ...) (LOG_INFO(module, BLACK, WHITE, __VA_ARGS__))
    #define LOGW(module, ...) (LOG_INFO(module, BLACK, BROWN, __VA_ARGS__))
    #define LOGE(module, ...) (LOG_INFO(module, BLACK, RED, __VA_ARGS__))
    #define LOGCE(module, ...) (LOG_INFO(module, RED, WHITE, __VA_ARGS__))
#else
    #define LOGINM(...) ( )
    #define LOGWNM(...) ( )
    #define LOGENM(...) ( )
    #define LOGCENM(...) ( )
    
    #define LOGI(module, ...) ( )
    #define LOGW(module, ...) ( )
    #define LOGC(module, ...) ( )
    #define LOGCE(module, ...) ( )    
#endif