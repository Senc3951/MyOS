#pragma once

#include <stdint.h>
#include <stdbool.h>

#define PIC1_COMMAND    0x20
#define PIC1_DATA       0x21
#define PIC2_COMMAND    0xA0
#define PIC2_DATA       0xA1

typedef enum PIC_ICW1
{
    PIC_ICW1_ICW4       = 0x01,
    PIC_ICW1_SINGLE     = 0x02,
    PIC_ICW1_INTERVAL4  = 0x04,
    PIC_ICW1_LEVEL      = 0x08,
    PIC_ICW1_INITIALIZE = 0x10
} PIC_ICW1_t;

typedef enum PIC_ICW4
{
    PIC_ICW4_8086           = 0x1,
    PIC_ICW4_AUTO_EOI       = 0x2,
    PIC_ICW4_BUFFER_MASTER  = 0x0C,
    PIC_ICW4_BUFFER_SLAVE   = 0x08,
    PIC_ICW4_SFNM           = 0x10
} PIC_ICW4_t;

typedef enum PIC_CMD
{
    PIC_EOI         = 0x20,
    PIC_READ_IRR    = 0x0A,
    PIC_READ_ISR    = 0x0B
} PIC_CMD_t;

/**
 * @brief Initialize the PIC component.
 * 
 * @param offsetPIC1 Vector offset for Master PIC.
 * @param offsetPIC2 Vector offset for Slave PIC.
 * @param autoEOI Automatically send EOI after interrupt.
*/
void PIC_Initialize(const uint8_t offsetPIC1, const uint8_t offsetPIC2, const bool autoEOI);

/**
 * @brief Disable all IRQ pins.
*/
void PIC_Disable();

/**
 * @brief Mask an IRQ pin.
 * 
 * @param irq IRQ to mask.
*/
void PIC_Mask(uint8_t irq);

/**
 * @brief Unmask an IRQ pin.
 * 
 * @param irq IRQ to unmask.
*/
void PIC_Unmask(uint8_t irq);

/**
 * @brief Send an EOI to an IRQ pin.
 * 
 * @param irq pin to send the EOI to.
*/
void PIC_SendEOI(const uint8_t irq);

/**
 * @brief Get the Combined value of the PIC's IRR.
 * 
 * @return Combined value of the PIC's IRR.
*/
uint16_t PIC_GetIRR();

/**
 * @brief Get the Combined value of the PIC's ISR.
 * 
 * @return Combined value of the PIC's ISR.
*/
uint16_t PIC_GetISR();