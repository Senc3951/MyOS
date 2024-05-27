#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>

#define __PACKED__      __attribute__((packed))
#define __NO_RETURN__   __attribute__((noreturn))
#define __MALLOC__      __attribute__((malloc))
#define __ALIGNED__(n)  __attribute__((aligned(n)))

#define MIN(x, y)           ((x) < (y) ? (x) : (y))
#define MAX(x, y)           ((x) > (y) ? (x) : (y))
#define UNUSED(x)           (void)(x)

#define rndup(num, size)    (((num) + (size) - 1) & ~((size) - 1))
#define rndown(num, size)   ((num) & ~((size) - 1))

#define _KB (1024ULL)
#define _MB (_KB * 1024)
#define _GB (_MB * 1024)

extern const uintptr_t _KERNEL_START;
extern const uintptr_t _KERNEL_END;
extern const uintptr_t _KERNEL_WRITE_PROTECT_START;
extern const uintptr_t _KERNEL_WRITE_PROTECT_END;