#pragma once

#include "tss.h"
#include <stddef.h>

#define ENTRIES 6
#define KERNEL_CS 0x08
#define KERNEL_DS 0x10

typedef enum GDTFLAGS
{
    GDT_READ_WRITE              = 0x2,
    GDT_EXECUTABLE              = 0x8,
    GDT_FLAGS                   = 0xC,
    GDT_SEGMENT                 = 0x10,
    GDT_LONG_MODE_GRANULARITY   = 0x20,
    GDT_USER                    = 0x60,
    GDT_PRESENT                 = 0x80
} GDTFlags_t;

#define GET_GDT_LIMIT_LOW(limit)                (limit & 0xFFFF)
#define GET_GDT_BASE_LOW(base)                  (base & 0xFFFF)
#define GET_GDT_BASE_MIDDLE(base)               ((base >> 16) & 0xFF)
#define GET_GDT_GRANULARITY(limit, granularity) (((limit >> 16) & 0xF) | (granularity & 0xF0))
#define GET_GDT_BASE_HIGH(base)                 ((base >> 24) & 0xFF)

typedef struct GDTEntry
{
    uint16_t limitLow;
    uint16_t baseLow;
    uint8_t baseMiddle;
    uint8_t access;
    uint8_t granularity;
    uint8_t baseHigh;
} __attribute__((packed)) GDTEntry_t;

typedef struct GDTEntries
{
    GDTEntry_t entries[ENTRIES];
    TSSEntry_t tss;
} __attribute__((packed)) GDTEntries_t;

typedef struct GlobalDescriptorTable
{
    uint16_t limit;
    uint64_t base;
} __attribute__((packed)) GlobalDescriptorTable_t;

/**
 * @brief Load the GDT.
*/
void GDT_Initialize();