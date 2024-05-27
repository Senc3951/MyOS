#include <boot/mb2.h>
#include <panic.h>
#include <assert.h>
#include <libc/string.h>

#define ALIGN_SIZE(size)    ((size + 7) & ~7)
#define ITER_MB2()          for (struct multiboot_tag *tag = (struct multiboot_tag *)(g_info + 1); tag->type != MULTIBOOT_TAG_TYPE_END; tag = (struct multiboot_tag *)((uint8_t *)tag + ALIGN_SIZE(tag->size)))

static const multiboot_info_t *g_info = NULL;
static struct multiboot_tag_module *g_modules[MB2_MAX_MODULES];
static size_t g_moduleCount;

void mb2_init(const uint32_t magic, const multiboot_info_t *info)
{
    assert(info);
    g_info = info;
    g_moduleCount = 0;
    
    // Validate loaded by multiboot
    assert(magic == MULTIBOOT2_BOOTLOADER_MAGIC);
    assert(!(*(uintptr_t *)info & 7));

    // Find all modules
    ITER_MB2()
    {
        if (tag->type == MULTIBOOT_TAG_TYPE_MODULE)
            g_modules[g_moduleCount++] = (struct multiboot_tag_module *)tag;
    }
}

void *mb2_get_tag(const uint32_t type)
{    
    ITER_MB2()
    {
        if (tag->type == type)
            return (void *)tag;
    }
    
    panic("multiboot2 tag %u was not found", type);
}

struct multiboot_tag_module *mb2_find_module(const char *name)
{
    for (size_t i = 0; i < g_moduleCount; i++)
    {
        if (!strcmp(g_modules[i]->cmdline, name))
            return g_modules[i];
    }
    
    panic("multiboot2 module %s was not found", name);
}