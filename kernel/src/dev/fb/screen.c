#include <dev/fb/screen.h>
#include <dev/fb/psf.h>
#include <boot/mb2.h>
#include <mem/vmm.h>
#include <arch/cpu.h>
#include <libc/string.h>
#include <log.h>

static struct multiboot_tag_framebuffer_common *g_framebuffer;
static struct multiboot_tag_framebuffer *g_framebufferShift;
static uint32_t g_bytesPerPixel, g_rows, g_cols;
static uint32_t g_offset = 0, g_bg, g_fg;
static psf_font_t g_fontHeader;
static void *g_fontGlyph = NULL;
static bool g_initialized = false;

#define DRAW_PIXEL(a, c)    (*(uint32_t *)(a) = (c))
#define GET_INIT_ADDR(x, y) ((uint64_t)(g_framebuffer->framebuffer_addr + (y) * g_framebuffer->framebuffer_pitch + (x) * g_bytesPerPixel))

#define GET_COLOR(c) ({                 \
    (uint32_t)((uint32_t)c->red << g_framebufferShift->framebuffer_red_field_position |     \
    (uint32_t)c->green << g_framebufferShift->framebuffer_green_field_position |            \
    (uint32_t)c->blue << g_framebufferShift->framebuffer_blue_field_position);              \
})

static inline void drawc(char c)
{
    size_t row = g_offset / g_cols;
    uint32_t x = (g_offset % g_cols) * g_fontHeader.width;
    uint32_t y = row * g_fontHeader.headerSize;

    uint32_t xmax = MIN(x + g_fontHeader.width, g_framebuffer->framebuffer_width);
    uint32_t ymax = MIN(y + g_fontHeader.height, g_framebuffer->framebuffer_height);
    uint8_t *glyph = g_fontGlyph + (g_fontHeader.bytesPerGlyph * (uint8_t)c);

    for (uint32_t y1 = y; y1 < ymax; y1++)
    {
        uint64_t addr = GET_INIT_ADDR(x, y1);
        for (uint32_t x1 = x; x1 < xmax; x1++)
        {
            uint8_t gb = *(glyph + (y1 - y));   
            
            DRAW_PIXEL(addr, (gb >> (8 - (x1 - x))) & 1 ? g_fg : g_bg);
            addr += g_bytesPerPixel;
        }
    }
    
    g_offset++;
}

static inline void delc()
{
    if (!g_offset)
        return;

    g_offset--;

    size_t row = g_offset / g_cols;
    uint32_t x = (g_offset % g_cols) * g_fontHeader.width;
    uint32_t y = row * g_fontHeader.headerSize;

    uint32_t xmax = MIN(x + g_fontHeader.width, g_framebuffer->framebuffer_width);
    uint32_t ymax = MIN(y + g_fontHeader.height, g_framebuffer->framebuffer_height);
 
    for (uint32_t y1 = y; y1 < ymax; y1++)
    {
        uint64_t addr = GET_INIT_ADDR(x, y1);
        for (uint32_t x1 = x; x1 < xmax; x1++)
        {
            DRAW_PIXEL(addr, g_bg);
            addr += g_bytesPerPixel;
        }
    }
}

void screen_init()
{
    g_framebufferShift = (struct multiboot_tag_framebuffer *)mb2_get_tag(MULTIBOOT_TAG_TYPE_FRAMEBUFFER);
    g_framebuffer = &g_framebufferShift->common;
    g_bytesPerPixel = g_framebuffer->framebuffer_bpp / 8;
    
    // Map framebuffer
    size_t pages = rndup(g_framebuffer->size, PAGE_SIZE);
    uint64_t framebufferRoundedAddress = rndown(g_framebuffer->framebuffer_addr, PAGE_SIZE);
    pt_imapRange(get_kernel_pd(), framebufferRoundedAddress, framebufferRoundedAddress + pages * PAGE_SIZE, PAGING_READ_WRITE | PAGING_CACHE_WC);
    __flush_tlb(framebufferRoundedAddress);
    
    // Load font
    psf_read_font(FONT_FILENAME, &g_fontHeader, &g_fontGlyph);
    g_cols = g_framebuffer->framebuffer_width / g_fontHeader.width;
    g_rows = g_framebuffer->framebuffer_height / g_fontHeader.height;
    
    // Prepare screen
    g_initialized = true;
    screen_clear(&BLACK);
    screen_set_colors(&BLACK, &WHITE);
    
    LOG_WARNING_INFO("Framebuffer at %llx (%ux%ux%u)\n", g_framebuffer->framebuffer_addr, g_framebuffer->framebuffer_height, g_framebuffer->framebuffer_width, g_framebuffer->framebuffer_bpp);
}

void screen_putc(int c)
{
    if (!g_initialized)
        return;
    if (c <= 0)
        return;
    
    switch (c)
    {
    case '\n':
        uint32_t row = g_offset / g_cols;
        g_offset = (row + 1) * g_cols;
        
        break;
    case '\t':
        g_offset += 4;
        break;
    case '\b':
        delc();
        break;
    default:
        drawc(c);
        break;
    }
}

void screen_set_colors(const RGB_t *bg, const RGB_t *fg)
{
    if (bg)
        g_bg = GET_COLOR(bg);
    if (fg)
        g_fg = GET_COLOR(fg);
}

void screen_clear(const RGB_t *color)
{
    if (!g_initialized)
        return;
    
    g_offset = 0;
    screen_fillrect(0, 0, g_framebuffer->framebuffer_width, g_framebuffer->framebuffer_height, color);   
}

void screen_fillrect(const uint32_t x, const uint32_t y, const uint32_t w, const uint32_t h, const RGB_t *color)
{
    if (!g_initialized)
        return;
    
    uint32_t c = GET_COLOR(color);
    for (uint32_t i = x; i < x + w; i++)
    {
        uint64_t addr = GET_INIT_ADDR(i, y);
        for (uint32_t j = y; j < y + h; j++)
        {
            DRAW_PIXEL(addr, c);
            addr += g_framebuffer->framebuffer_pitch;
        }
    }
}