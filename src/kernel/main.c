#include <io/io.h>
#include <io/vga.h>
#include <sys/hal.h>
#include <mem/mmu.h>
#include <io/serial.h>
#include <sys/fxsave.h>

extern void enable_sse();

void kmain(uintptr_t magic, struct multiboot2_info *mbTags)
{
    CLI();
    
    enable_sse();
    fxsave_init();
    
    serial_init();
    vga_clrsn();
    
    mmu_init(magic, mbTags);
    hal_init();

    while (1)   // Temporary
        CLI();
}