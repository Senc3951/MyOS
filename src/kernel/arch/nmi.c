#include <io/io.h>
#include <arch/nmi.h>

void nmi_enable()
{
    uint8_t d = inb(0x70);
    IO_WAIT();

    outb(0x70, d & 0x7F);
    IO_WAIT();
    
    inb(0x71);
}

void nmi_disable()
{
    uint8_t d = inb(0x70);
    IO_WAIT();
    
    outb(0x70, d | 0x80);
    IO_WAIT();
    
    inb(0x71);
}