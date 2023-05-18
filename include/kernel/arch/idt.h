#pragma once

#include <stdint.h>

#define IDT_GATES 256
#define IDT_INTERRUPT_GATE  0x8E
#define IDT_TRAP_GATE       0x8F

struct idt_entry
{
    uint16_t offsetLow;
    uint16_t segment;
    uint8_t empty;
    uint8_t flags;
    uint16_t offsetMid;
    uint32_t offsetHigh;
    uint32_t reserved;
} __attribute__((packed));

struct idt
{
    uint16_t limit;
    struct idt_entry *base;
} __attribute__((packed));

/**
 * @brief Initialize the IDT.
*/
void idt_init();

/**
 * @brief Set an IDT entry.
 * 
 * @param gateNumber Index in the IDT table.
 * @param offset Entry's offset.
 * @param segment Entry's segment.
 * @param flags Entry's flags.
*/
void idt_set_entry(const uint8_t gateNumber, const uint64_t offset, const uint16_t segment, const uint8_t flags);