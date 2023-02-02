#pragma once

#include <stdint.h>

#define GDT_TSS_PRESENT 0x80

typedef struct TSSEntry
{
    uint16_t size;
    uint16_t base_low;
    uint8_t base_mid;
    uint8_t flags1;
    uint8_t flags2;
    uint8_t base_high;
    uint32_t base_upper;
    uint32_t reserved0;
} __attribute__((packed)) TSSEntry_t;

typedef struct TaskStateSegment
{
    uint32_t reserved0;
    uint64_t rsp[3];
    uint64_t reserved1;
    uint64_t ist[7];
    uint32_t reserved2;
    uint32_t reserved3;
    uint16_t reserved4;
    uint16_t iomap_base;
} __attribute__((packed)) TaskStateSegment_t;