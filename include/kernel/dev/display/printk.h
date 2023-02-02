#pragma once

// Taken from https://github.com/hrinn/HaydenOS/blob/0c83ed94da193881c4b278b16e8df4b4d7237ab6/src/kernel/lib/printk.c

#include "vga.h"
#include <kernel/lib/std.h>

#define FORMAT_BUFF 20

typedef struct PRINTK_FORMAT_INFO
{
    uint8_t length;
    char length_specifier;
    char format_specifier;
} PrintkFormatInfo_t;

bool K_CheckFormatSpecifier(const char *fmt, PrintkFormatInfo_t *info)
{
    if (*fmt != '%') 
        return false;
    
    // Check the length specifier
    switch (*(fmt + 1))
    {
        case 'h':
        case 'l':
        case 'q':
            info->length_specifier = *(++fmt);
            break;
        default:
            break;
    }

    // Check the format specifier supports length specifiers
    if (info->length_specifier)
    {
        switch (*(fmt + 1))
        {
            case 'd':
            case 'u':
            case 'x':
            case 'X':
                info->length = 3;
                info->format_specifier = *(fmt + 1);
                return true;
            default:
                return false;
        }
    }

    // Check the format specifier
    switch (*(fmt + 1))
    {
        case '%':
        case 'd':
        case 'u':
        case 'x':
        case 'X':
        case 'c':
        case 'p':
        case 's':
            info->length = 2;
            info->format_specifier = *(fmt + 1);
            return true;
        default:
            return false;
    }
}

void K_Print(const char *buff, size_t len)
{
    for (size_t i = 0; i < len && buff[i] != '\0'; i++)
        putc(buff[i]);
}

void K_PrintInt(int64_t i)
{
    char buffer[FORMAT_BUFF];
    itoa(i, buffer, 10, false);
    
    K_Print(buffer, FORMAT_BUFF);
}

void K_PrintUInt(uint64_t u)
{
    char buffer[FORMAT_BUFF];
    uitoa(u, buffer, 10, false);

    K_Print(buffer, FORMAT_BUFF);
}

void K_PrintHex(uint64_t h, bool caps)
{
    char buffer[FORMAT_BUFF];
    uitoa(h, buffer, 16, caps);

    K_Print(buffer, FORMAT_BUFF);
}