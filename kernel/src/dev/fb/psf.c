#include <dev/fb/psf.h>
#include <boot/mb2.h>
#include <mem/vmm.h>
#include <libc/string.h>
#include <panic.h>

void psf_read_font(const char *filename, psf_font_t *header, void **glyph)
{
    // Identity map module
    struct multiboot_tag_module *module = mb2_find_module(filename);
    pt_imapRange(get_kernel_pd(), module->mod_start, module->mod_end, PAGING_READ_WRITE);

    // Read header
    memcpy(header, (const void *)module->mod_start, sizeof(psf_font_t));
    if (header->magic != PSF_FONT_MAGIC)
        panic("psf font file %s is corrupted", filename);
    
    // Glyph after header
    *glyph = (void *)(module->mod_start + sizeof(psf_font_t));
}