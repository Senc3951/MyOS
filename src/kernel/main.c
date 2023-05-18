#include <io/io.h>
#include <sys/hal.h>
#include <dev/display/vga.h>
#include <dev/display/serial.h>

void kmain(uintptr_t magic, void *mbTags)
{
    CLI();
    
    serial_init();
    vga_clrsn();
    hal_init();

    while (1)   // Temporary
        CLI();
}