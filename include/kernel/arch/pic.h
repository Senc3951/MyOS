#pragma once

#include <stdint.h>
#include <stdbool.h>

#define PIC1_COMMAND    0x20
#define PIC1_DATA       0x21
#define PIC2_COMMAND    0xA0
#define PIC2_DATA       0xA1

enum pic_icw1
{
    PIC_ICW1_ICW4       = 0x01,
    PIC_ICW1_SINGLE     = 0x02,
    PIC_ICW1_INTERVAL4  = 0x04,
    PIC_ICW1_LEVEL      = 0x08,
    PIC_ICW1_INITIALIZE = 0x10
};

enum pic_icw4
{
    PIC_ICW4_8086           = 0x1,
    PIC_ICW4_AUTO_EOI       = 0x2,
    PIC_ICW4_BUFFER_MASTER  = 0x0C,
    PIC_ICW4_BUFFER_SLAVE   = 0x08,
    PIC_ICW4_SFNM           = 0x10
};

enum pic_cmd
{
    PIC_EOI         = 0x20,
    PIC_READ_IRR    = 0x0A,
    PIC_READ_ISR    = 0x0B
};

/**
 * @brief Initialize the PIC.
 * 
 * @param offsetPIC1 Vector offset for Master PIC.
 * @param offsetPIC2 Vector offset for Slave PIC.
 * @param autoEOI Automatically send EOI after interrupt.
*/
void pic_init(const uint8_t offsetPIC1, const uint8_t offsetPIC2, const bool autoEOI);

/**
 * @brief Disable all IRQ pins.
*/
void pic_disable();

/**
 * @brief Mask an IRQ pin.
 * 
 * @param irq IRQ to mask.
*/
void pic_mask(uint8_t irq);

/**
 * @brief Unmask an IRQ pin.
 * 
 * @param irq IRQ to unmask.
*/
void pic_unmask(uint8_t irq);

/**
 * @brief Send an EOI to an IRQ pin.
 * 
 * @param irq pin to send the EOI to.
*/
void pic_send_eoi(const uint8_t irq);