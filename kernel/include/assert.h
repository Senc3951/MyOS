#pragma once

#include <common.h>

void __NO_RETURN__ __assertation_failed(const char *expr, const char *file, const char *function, uint32_t line);

#define assert(expr) {                                                  \
    if (!(expr))                                                        \
        __assertation_failed(#expr, __FILE__, __FUNCTION__, __LINE__);  \
}
