#include <kernel/io/io.h>
#include <kernel/mem/mmu.h>
#include <kernel/arch/hal.h>
#include <kernel/dev/display/vga.h>

void kmain(uintptr_t magic, MB2Info_t *mbTags)
{
    clrscn();
    __CLI();
    
    HAL_Initialize();
    MMU_Initialize(magic, mbTags);
    
    while (1)
        __HALT();
}