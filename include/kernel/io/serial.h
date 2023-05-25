#pragma once

#include <stdint.h>
#include <stdbool.h>

#define SERIAL_PORT 0x3F8

/**
 * @brief Initialize the serial port.
*/
bool serial_init();

/**
 * @brief Read a character from the serial port.
 * 
 * @return Character that was read.
*/
uint8_t serial_read();

/**
 * @brief Write a character to the serial port.
 * 
 * @param c Character to write.
*/
void serial_write(uint8_t c);