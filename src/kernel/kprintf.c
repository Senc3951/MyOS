#include <kprintf.h>
#include <lib/std.h>
#include <dev/display/vga.h>
#include <dev/display/serial.h>

// To avoid if checks every char we wish to print
void (*g_putcFunc)(uint8_t c) = NULL;

static void putc(uint8_t c)
{
    g_putcFunc(c);
}

static void sputc(uint8_t c)
{
    serial_write(c);
}

static void aputc(uint8_t c)
{
    serial_write(c);
    vga_putc(c);
}

static void puts(const uint8_t *s)
{
    size_t i = 0;
    while (s[i] != '\0')
        putc(s[i++]);
}

static void print(const uint8_t *s, size_t len)
{
    for (size_t i = 0; i < len && s[i] != '\0'; i++)
        putc(s[i]);
}

#define print_int(num) ({ \
    char buffer[FORMAT_BUFF]; \
    itoa(num, buffer, 10, false); \
    \
    print(buffer, FORMAT_BUFF); \
})

#define print_uint(num) ({ \
    char buffer[FORMAT_BUFF]; \
    uitoa(num, buffer, 10, false); \
    \
    print(buffer, FORMAT_BUFF); \
})

#define print_double(num) ({ \
    char buffer[FORMAT_BUFF * 2]; \
    ftoa(num, buffer, 3); \
    \
    print(buffer, FORMAT_BUFF * 2); \
})

#define print_hex(num, caps) ({ \
    char buffer[FORMAT_BUFF]; \
    uitoa(num, buffer, 16, caps); \
    \
    print(buffer, FORMAT_BUFF); \
})

static bool check_format_specifier(const char *fmt, struct kprintf_format_info *info)
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
        case 'f':
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

int vkprintf(const char *fmt, va_list valist)
{
    int start = 0, end = 0;
    struct kprintf_format_info info;
    bool format_flag, end_flag;
    
    if (fmt == NULL)
        return -1;

    do
    {
        info.length = 0;
        info.format_specifier = '\0';
        info.length_specifier = '\0';
        format_flag = check_format_specifier(fmt + end, &info);
        end_flag = end_flag = (fmt[end] == '\0');
        
        if (end_flag || format_flag)
            print(fmt + start, end - start);
        
        if (format_flag)
        {
            switch (info.format_specifier)
            {
                case '%':
                    putc('%');
                    break;
                case 'd': // Signed integer
                    switch (info.length_specifier)
                    {
                        case 'h': print_int((int16_t)va_arg(valist, int32_t));          break;
                        case 'l': print_int(va_arg(valist, int64_t));                   break;
                        case 'q': print_int(va_arg(valist, int64_t));                   break;
                        case '\0': print_int(va_arg(valist, int32_t));                  break;
                    }
                    break;
                case 'u': // Unsigned integer
                    switch (info.length_specifier)
                    {
                        case 'h': print_uint((uint16_t)va_arg(valist, uint32_t));       break;
                        case 'l': print_uint(va_arg(valist, uint64_t));                 break;
                        case 'q': print_uint(va_arg(valist, uint64_t));                 break;
                        case '\0': print_uint(va_arg(valist, uint32_t));                break;
                    }
                    break;
                case 'f': // Float
                    print_double(va_arg(valist, double));
                    break;
                case 'x': // Lowercase hex
                    switch (info.length_specifier)
                    {
                        case 'h': print_hex((uint16_t)va_arg(valist, uint32_t), false); break;
                        case 'l': print_hex(va_arg(valist, uint64_t), false);           break;
                        case 'q': print_hex(va_arg(valist, uint64_t), false);           break;
                        case '\0': print_hex(va_arg(valist, uint32_t), false);          break;
                    }
                    break;
                case 'X': // Uppercase hex
                    switch (info.length_specifier)
                    {
                        case 'h': print_hex((uint16_t)va_arg(valist, uint32_t), true);  break;
                        case 'l': print_hex(va_arg(valist, uint64_t), true);            break;
                        case 'q': print_hex(va_arg(valist, uint64_t), true);            break;
                        case '\0': print_hex(va_arg(valist, uint32_t), true);           break;
                    }
                    break;
                case 'c': // Char
                    putc((char)va_arg(valist, int));
                    break;
                case 'p': // Pointer address
                    print_hex(va_arg(valist, uint64_t), false);
                    break;
                case 's': // String
                    puts(va_arg(valist, char *));
                    break;
                default: // Invalid specifier
                    return -1;
            }
            
            end += info.length;
            start = end--;
        }
    } while (fmt[end++]);
    
    return 1;
}

int kprintf(const char* fmt, ...)
{
    g_putcFunc = aputc;
    va_list args;
    va_start(args, fmt);
    
    return vkprintf(fmt, args);
}

int ksprintf(const char *fmt, ...)
{
    g_putcFunc = sputc;
    va_list args;
    va_start(args, fmt);
    
    return vkprintf(fmt, args);
}