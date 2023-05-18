#pragma once

#include <stdint.h>

struct SYMBOL
{
    uint64_t addr;
    char *name;
} __attribute__((packed));

extern const struct SYMBOL kernel_symbols[];