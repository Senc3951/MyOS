#pragma once

#include "tss.h"
#include <stddef.h>

#define ENTRIES 6
#define KERNEL_CS 0x08
#define KERNEL_DS 0x10

#define GDT_READ_WRITE              0x2
#define GDT_EXECUTABLE              0x8
#define GDT_FLAGS                   0xC
#define GDT_SEGMENT                 0x10
#define GDT_LONG_MODE_GRANULARITY   0x20
#define GDT_USER                    0x60
#define GDT_PRESENT                 0x80

#define GDT_GET_LIMIT_LOW(limit)                (limit & 0xFFFF)
#define GDT_GET_BASE_LOW(base)                  (base & 0xFFFF)
#define GDT_GET_BASE_MIDDLE(base)               ((base >> 16) & 0xFF)
#define GDT_GET_GRANULARITY(limit, gran)        (((limit >> 16) & 0xF) | (gran & 0xF0))
#define GDT_GET_BASE_HIGH(base)                 ((base >> 24) & 0xFF)

struct gdt_entry
{
    uint16_t limitLow;
    uint16_t baseLow;
    uint8_t baseMiddle;
    uint8_t access;
    uint8_t granularity;
    uint8_t baseHigh;
} __attribute__((packed));

struct gdt_entries
{
    struct gdt_entry entries[ENTRIES];
    struct tss_entry tss;
} __attribute__((packed));

struct gdt
{
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

/**
 * @brief Load the GDT.
*/
void gdt_init();