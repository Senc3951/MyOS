#pragma once

#include <common.h>

#define PSF_FONT_MAGIC 0x864ab572

typedef struct PSF_FONT
{
    uint32_t magic;
    uint32_t version;
    uint32_t headerSize;
    uint32_t flags;
    uint32_t glyphNum;
    uint32_t bytesPerGlyph;
    uint32_t height;
    uint32_t width;
} psf_font_t;

void psf_read_font(const char *filename, psf_font_t *header, void **glyph);