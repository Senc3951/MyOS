#pragma once

// Taken from https://github.com/hrinn/HaydenOS/blob/0c83ed94da193881c4b278b16e8df4b4d7237ab6/src/kernel/lib/printk.c

#include <stdint.h>
#include <stdarg.h>

#define FORMAT_BUFF 20

struct kprintf_format_info
{
    uint8_t length;
    char length_specifier;
    char format_specifier;
};

/**
 * @brief Print formatted string with arguments.
 * 
 * @param fmt Format to print.
 * @param args Arguments to print.
 * @return 1 if executed successfully, -1 otherwise.
*/
int vkprintf(const char *fmt, va_list args);

/**
 * @brief Print a formatted string.
 * 
 * @param fmt Format to print.
 * @return 1 if executed successfully, -1 otherwise.
*/
int kprintf(const char* fmt, ...);

/**
 * @brief Print a formatted string to the serial port.
 * 
 * @param fmt Format to print.
 * @return 1 if executed successfully, -1 otherwise.
*/
int ksprintf(const char *fmt, ...);