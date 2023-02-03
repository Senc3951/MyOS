#include <kernel/io/io.h>
#include <kernel/lib/string.h>
#include <kernel/dev/display/vga.h>
#include <kernel/dev/display/printk.h>

#define GET_INDEX(r, c) (r * WIDTH + c)
#define GET_VGA_COLOR(back, fore) (fore | back << 4)
#define GET_VGA_CHAR(c) ((uint16_t)c | (uint16_t)g_Color << 8)

volatile uint16_t *g_VideoBuffer = (uint16_t *)0xb8000;
uint16_t g_Row = 0, g_Col = 0;
bool g_UpdateCursor = false;
uint8_t g_Color = GET_VGA_COLOR(BLACK, WHITE), g_ForeColor = WHITE;

static inline void scroll()
{
    memcpy((void *)g_VideoBuffer, (void *)(g_VideoBuffer + WIDTH), ((HEIGHT - 1) * WIDTH) * 2);
    memset((void *)(g_VideoBuffer + ((HEIGHT - 1) * WIDTH)), 0, WIDTH * 2);

    g_Row--;
}

static inline void updateCursor()
{
    if (!g_UpdateCursor)
        return;
    
    uint16_t pos = g_Row * WIDTH + g_Col;
	outb(0x3D4, 0x0F);
	outb(0x3D5, pos & 0xFF);
	outb(0x3D4, 0x0E);
	outb(0x3D5, (pos >> 8) & 0xFF);
}

static void ptc(const uint8_t c, const bool isString);

void VGA_EnableCursor()
{
    g_UpdateCursor = true;
    outb(0x3D4, 0x0A);
	outb(0x3D5, (inb(0x3D5) & 0xC0));

	outb(0x3D4, 0x0B);
	outb(0x3D5, (inb(0x3D5) & 0xE0) | 15);
    
    updateCursor();
}

void VGA_DisableCursor()
{
    g_UpdateCursor = false;
    outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);
}

void clrscnc(const VGAColor_t background)
{
    g_Color = GET_VGA_COLOR(background, g_ForeColor);
    for (size_t i = 0; i < HEIGHT; i++)
    {
        for (size_t j = 0; j < WIDTH; j++)
            g_VideoBuffer[GET_INDEX(i, j)] = GET_VGA_CHAR(' ');
    }
        
    g_Row = g_Col = 0;
}

void clrscn()
{
    clrscnc(BLACK);
}

void setscnc(const VGAColor_t background, const VGAColor_t foreground)
{
    g_ForeColor = foreground;
    g_Color = GET_VGA_COLOR(background, foreground);
}

void delc()
{
    if (g_Col == 0)
    {
        g_Row--;
        g_Col = WIDTH;
    }
    if (g_Row == 0)
        return;
    
    g_VideoBuffer[GET_INDEX(g_Row, --g_Col)] = GET_VGA_CHAR(' ');
}

void putc(const uint8_t c)
{
    ptc(c, false);
}

void ptc(const uint8_t c, const bool isString)
{
    switch (c)
    {
        case '\n':
            g_Row++; g_Col = 0;
            break;
        case '\t':
            for (uint8_t i = 0; i < TAB_LENGTH - (g_Col % TAB_LENGTH); i++)
                putc(' ');
            
            break;
        case '\r':
            g_Col = 0;
            break;
        default:
            g_VideoBuffer[GET_INDEX(g_Row, g_Col++)] = GET_VGA_CHAR(c);
            break;
    }

    if (g_Col == WIDTH)
    {
        g_Col = 0;
        g_Row++;
    }
    if (g_Row == HEIGHT)
        scroll();
    
    // If we are printing a string, dont update every character
    if (!isString)
        updateCursor();
}

void puts(const uint8_t *str)
{
    size_t i = 0;
    while (str[i] != '\0')
        ptc(str[i++], false);
    
    updateCursor();
}

int vkprintf(const char *fmt, va_list valist)
{
    int start = 0, end = 0;
    PrintkFormatInfo_t info;
    bool format_flag, end_flag;
    
    if (fmt == NULL)
        return -1;

    do
    {
        info.length = 0;
        info.format_specifier = '\0';
        info.length_specifier = '\0';
        format_flag = K_CheckFormatSpecifier(fmt + end, &info);
        end_flag = end_flag = (fmt[end] == '\0');
        
        if (end_flag || format_flag)
            K_Print(fmt + start, end - start);
        
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
                        case 'h': K_PrintInt((int16_t)va_arg(valist, int32_t));          break;
                        case 'l': K_PrintInt(va_arg(valist, int64_t));                   break;
                        case 'q': K_PrintInt(va_arg(valist, int64_t));                   break;
                        case '\0': K_PrintInt(va_arg(valist, int32_t));                  break;
                    }
                    break;
                case 'u': // Unsigned integer
                    switch (info.length_specifier)
                    {
                        case 'h': K_PrintUInt((uint16_t)va_arg(valist, uint32_t));       break;
                        case 'l': K_PrintUInt(va_arg(valist, uint64_t));                 break;
                        case 'q': K_PrintUInt(va_arg(valist, uint64_t));                 break;
                        case '\0': K_PrintUInt(va_arg(valist, uint32_t));                break;
                    }
                    break;
                case 'x': // Lowercase hex
                    switch (info.length_specifier)
                    {
                        case 'h': K_PrintHex((uint16_t)va_arg(valist, uint32_t), false); break;
                        case 'l': K_PrintHex(va_arg(valist, uint64_t), false);           break;
                        case 'q': K_PrintHex(va_arg(valist, uint64_t), false);           break;
                        case '\0': K_PrintHex(va_arg(valist, uint32_t), false);          break;
                    }
                    break;
                case 'X': // Uppercase hex
                    switch (info.length_specifier)
                    {
                        case 'h': K_PrintHex((uint16_t)va_arg(valist, uint32_t), true);  break;
                        case 'l': K_PrintHex(va_arg(valist, uint64_t), true);            break;
                        case 'q': K_PrintHex(va_arg(valist, uint64_t), true);            break;
                        case '\0': K_PrintHex(va_arg(valist, uint32_t), true);           break;
                    }
                    break;
                case 'c': // Char
                    putc((char)va_arg(valist, int));
                    break;
                case 'p': // Pointer address
                    K_PrintHex(va_arg(valist, uint64_t), false);
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
    va_list args;
    va_start(args, fmt);
    
    return vkprintf(fmt, args);
}