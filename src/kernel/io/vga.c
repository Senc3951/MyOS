#include <io/io.h>
#include <string.h>
#include <io/vga.h>

#define GET_INDEX(r, c) (r * WIDTH + c)
#define GET_VGA_COLOR(back, fore) (fore | back << 4)
#define GET_VGA_CHAR(c) ((uint16_t)c | (uint16_t)g_Color << 8)

volatile uint16_t *g_VideoBuffer = (uint16_t *)0xb8000;
uint16_t g_Row = 0, g_Col = 0;
uint8_t g_Color = GET_VGA_COLOR(BLACK, WHITE), g_ForeColor = WHITE;

static inline void scroll()
{
    memcpy((void *)g_VideoBuffer, (void *)(g_VideoBuffer + WIDTH), ((HEIGHT - 1) * WIDTH) * 2);
    memset((void *)(g_VideoBuffer + ((HEIGHT - 1) * WIDTH)), 0, WIDTH * 2);

    g_Row--;
}

void vga_clrcsn(const enum vga_color background)
{
    g_Color = GET_VGA_COLOR(background, g_ForeColor);
    for (size_t i = 0; i < HEIGHT; i++)
    {
        for (size_t j = 0; j < WIDTH; j++)
            g_VideoBuffer[GET_INDEX(i, j)] = GET_VGA_CHAR(' ');
    }
        
    g_Row = g_Col = 0;
}

void vga_clrsn()
{
    vga_clrcsn(BLACK);
}

void vga_setcsn(const enum vga_color background, const enum vga_color foreground)
{
    g_ForeColor = foreground;
    g_Color = GET_VGA_COLOR(background, foreground);
}

void vga_putc(const uint8_t c)
{
    switch (c)
    {
        case '\n':
            g_Row++; g_Col = 0;
            break;
        case '\t':
            for (uint8_t i = 0; i < TAB_LENGTH - (g_Col % TAB_LENGTH); i++)
                vga_putc(' ');
            
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
}

void vga_puts(const uint8_t *str)
{
    size_t i = 0;
    while (str[i] != '\0')
        vga_putc(str[i++]);
}

void vga_delc()
{
    if (g_Col == 0)
    {
        if (g_Row == 0)
            return;
        
        g_Row--;
        g_Col = WIDTH;
    }
    
    g_VideoBuffer[GET_INDEX(g_Row, --g_Col)] = GET_VGA_CHAR(' ');
}

void vga_delt()
{
    for (uint8_t i = 0; i < TAB_LENGTH - (g_Col % TAB_LENGTH); i++)
        vga_delc();
}