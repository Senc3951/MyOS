#pragma once

#include <stdint.h>

/**
 * @brief Write a Byte to an IO port.
 * 
 * @param port Port to write the value to.
 * @param value Value to write.
*/
void outb(const uint16_t port, const uint8_t value);

/**
 * @brief Write a Word to an IO port.
 * 
 * @param port Port to write the value to.
 * @param value value to wrote.
*/
void outw(const uint16_t port, const uint16_t value);

/**
 * @brief Write a DWord to an IO port.
 * 
 * @param port Port to write the value to.
 * @param value value to wrote.
*/
void outl(const uint16_t port, const uint32_t value);

/**
 * @brief Read a Byte from an IO port.
 * 
 * @param port Port to read from.
 * @return Value that was read from the port.
*/
uint8_t inb(const uint16_t port);

/**
 * @brief Read a Word from an IO port.
 * 
 * @param port Port to read from.
 * @return Value that was read from the port.
*/
uint16_t inw(const uint16_t port);

/**
 * @brief Read a DWord from an IO port.
 * 
 * @param port Port to read from.
 * @return Value that was read from the port.
*/
uint32_t inl(const uint16_t port);

/**
 * @brief Wait a couple of microseconds.
*/
#define IO_WAIT() { asm volatile("outb %%al, $0x80" : : "a"(0)); }

/**
 * @brief Disable interrupts.
*/
#define CLI() { asm("cli"); }

/**
 * @brief Enable interrupts.
*/
#define STI() { asm("sti"); }

/**
 * @brief Halt the processor.
*/
#define HALT() { asm("hlt"); }

/**
 * @brief Halt the processor while disabling interrupts.
 * 
 * @warning This will never return control to the caller.
*/
#define CLI_HALT() { while (1) asm("cli\n\thlt"); }