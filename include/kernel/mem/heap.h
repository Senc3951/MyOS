#pragma once

#include <stddef.h>

/**
 * @brief Allocate memory of size 'size'.
 * 
 * @param size Size in bytes of the allocated memory.
 * @return Address to the allocated memory, NULL if failed to do so.
*/
void *kmalloc(const size_t size);

/**
 * @brief Allocate zero'd memory of size 'size'.
 * 
 * @param size Size in bytes of the allocated memory.
 * @return Address to the allocated memory, NULL if failed to do so.
*/
void *kcalloc(const size_t size);

/**
 * @brief Free the allocated memory.
 * 
 * @param addr Address of the allocated memory.
*/
void kfree(void *addr);