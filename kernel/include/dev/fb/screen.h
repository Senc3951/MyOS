#pragma once

#include <common.h>

#define FONT_FILENAME "default_font"

typedef struct COLOR_STRUCT
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} RGB_t;

#define BLACK   (RGB_t){ 0, 0, 0 }
#define BLUE    (RGB_t){ 0, 0, 255 }
#define GREEN   (RGB_t){ 0, 255, 0 }
#define CYAN    (RGB_t){ 0, 255, 255 }
#define RED     (RGB_t){ 255, 0, 0 }
#define YELLOW  (RGB_t){ 255, 255, 0 }
#define WHITE   (RGB_t){ 255, 255, 255 }

void screen_init();

void screen_putc(int c);

void screen_set_colors(const RGB_t *bg, const RGB_t *fg);
void screen_clear(const RGB_t *color);
void screen_fillrect(const uint32_t x, const uint32_t y, const uint32_t w, const uint32_t h, const RGB_t *color);