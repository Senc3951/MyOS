#include <io/io.h>
#include <sys/hal.h>
#include <mem/mmu.h>
#include <sys/fxsave.h>
#include <dev/display/vga.h>
#include <dev/display/serial.h>

extern void enable_sse();

void kmain(uintptr_t magic, struct multiboot2_info *mbTags)
{
    CLI();
    
    enable_sse();
    fxsave_init();
    
    serial_init();
    vga_clrsn();
    hal_init();
    mmu_init(magic, mbTags);


    while (1)   // Temporary
        CLI();
}