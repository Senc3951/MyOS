#pragma once

#include <stdint.h>

/**
 * @brief Convert a signed int the a null terminated string.
 * 
 * @param num Int to convert to a string.
 * @param buffer Place to store the string at.
 * @param base Base to convert the number at.
 * @param caps Incase of hex, convert using Small or Capital letters.
*/
void itoa(int64_t num, char *buffer, const int base, const bool caps);

/**
 * @brief Convert an unsigned int the a null terminated string.
 * 
 * @param num Int to convert to a string.
 * @param buffer Place to store the string at.
 * @param base Base to convert the number at.
 * @param caps Incase of hex, convert using Small or Capital letters.
*/
void uitoa(uint64_t num, char *buffer, const int base, const bool caps);