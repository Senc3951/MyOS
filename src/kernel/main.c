#include <kernel/io/io.h>
#include <kernel/arch/hal.h>
#include <kernel/dev/display/vga.h>

void kmain(uintptr_t magic, void *mbTags)
{
    clrscn();
    HAL_Initialize();
    
    while (1)
        __HALT();
}