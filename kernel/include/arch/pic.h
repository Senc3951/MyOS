#pragma once

#include <common.h>

#define PIC1_COMMAND    0x20
#define PIC1_DATA       0x21
#define PIC2_COMMAND    0xA0
#define PIC2_DATA       0xA1

enum PIC_ICW1
{
    PIC_ICW1_ICW4       = 0x01,
    PIC_ICW1_SINGLE     = 0x02,
    PIC_ICW1_INTERVAL4  = 0x04,
    PIC_ICW1_LEVEL      = 0x08,
    PIC_ICW1_INITIALIZE = 0x10
};

enum PIC_ICW4
{
    PIC_ICW4_8086           = 0x1,
    PIC_ICW4_AUTO_EOI       = 0x2,
    PIC_ICW4_BUFFER_MASTER  = 0x0C,
    PIC_ICW4_BUFFER_SLAVE   = 0x08,
    PIC_ICW4_SFNM           = 0x10
};

enum PIC_CMD
{
    PIC_EOI         = 0x20,
    PIC_READ_IRR    = 0x0A,
    PIC_READ_ISR    = 0x0B
};

void pic_init(const uint8_t offset1, const uint8_t offset2, const bool autoEOI);
void pic_disable();

void pic_mask(uint8_t irq);
void pic_unmask(uint8_t irq);
void pic_eoi(const uint8_t irq);