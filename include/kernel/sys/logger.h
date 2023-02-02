#pragma once

#include <kernel/dev/display/vga.h>

#define DEBUG

#ifdef DEBUG
    #define LOG_INFO(pcp, b, f, ...) ({ \
        setscnc(b, f); \
        if (pcp) \
            kprintf("[%s:%s:%d] ", __FILE__, __FUNCTION__, __LINE__); \
        \
        kprintf(__VA_ARGS__); \
        setscnc(BLACK, WHITE); \
    })
    
    #define LOGI(pcp, ...) { LOG_INFO(pcp, BLACK, WHITE, __VA_ARGS__); }
    #define LOGW(pcp, ...) { LOG_INFO(pcp, BLACK, BROWN, __VA_ARGS__); }
    #define LOGE(pcp, ...) { LOG_INFO(pcp, BLACK, RED, __VA_ARGS__); }
    #define LOGCE(pcp, ...) { LOG_INFO(pcp, RED, WHITE, __VA_ARGS__); }
#else
    #define LOGI(pcp, ...) { }
    #define LOGW(pcp, ...) { }
    #define LOGC(pcp, ...) { }
    #define LOGCE(pcp, ...) { }    
#endif