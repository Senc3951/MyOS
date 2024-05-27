#pragma once

#include <common.h>

void __NO_RETURN__ __panic(const char *file, const char *function, uint32_t line, const char *fmt, ...);

#define panic(...) __panic(__FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)