#pragma once

#include <stdint.h>
#include <stddef.h>

/**
 * @brief Compare two memory addresses.
 * 
 * @param d1 First address.
 * @param d2 Second address.
 * @param n Amount of bytes to compare.
 * @return 1 if d1 is bigger than d2, -1 if d2 is bigger than d1 and 0 if they are equal.
*/
int memcmp(const void *d1, const void *d2, const size_t n);

/**
 * @brief Copy n bytes from destination to source.
 * 
 * @param dest Address to copy to.
 * @param src Address to copy from.
 * @param n Bytes to copy.
*/
void memcpy(void *dest, const void *src, const size_t n);

/**
 * @brief Set n bytes in destination as a character.
 * 
 * @param dest Destination to set the bytes at.
 * @param c Character to set.
 * @param n Bytes to set.
*/
void *memset(void *dest, const uint8_t c, size_t n);

/**
 * @brief Copy a string to another string.
 * 
 * @param dst Destination buffer.
 * @param src String to copy.
 * @param n Amount of bytes to copy.
 * @return Copied string.
*/
char *strncpy(char *dest, const char *src, size_t n);

/**
 * @brief Calculate the length of a string.
 * 
 * @param s String to calculate it's length.
 * @return Length of the string.
*/
size_t strlen(const char *s);

/**
 * @brief Reverse a string.
 * 
 * @param s String to reverse.
*/
void strrev(char *s);