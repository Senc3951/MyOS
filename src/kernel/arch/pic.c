#include <io/io.h>
#include <arch/pic.h>

static bool g_AutoEOI = false;

void pic_init(const uint8_t offsetPIC1, const uint8_t offsetPIC2, const bool autoEOI)
{
    // Initialize Control Word 1
    outb(PIC1_COMMAND, PIC_ICW1_ICW4 | PIC_ICW1_INITIALIZE);
    IO_WAIT();
    outb(PIC2_COMMAND, PIC_ICW1_ICW4 | PIC_ICW1_INITIALIZE);
    IO_WAIT();
    
    // Initialize Control Word 2
    outb(PIC1_DATA, offsetPIC1);
    IO_WAIT();
    outb(PIC2_DATA, offsetPIC2);
    IO_WAIT();
    
    // Initialize Control Word 3
    outb(PIC1_DATA, 4);    // Tell the Master that there is a Slave at IRQ2
    IO_WAIT();
    outb(PIC2_DATA, 2);
    IO_WAIT();
    
    uint8_t icw4 = PIC_ICW4_8086;
    if (autoEOI)
        icw4 |= PIC_ICW4_AUTO_EOI;
    
    // Initialize Control Word 4
    outb(PIC1_DATA, icw4);
    IO_WAIT();
    outb(PIC2_DATA, icw4);
    IO_WAIT();

    pic_disable();
}

void pic_disable()
{
    outb(PIC1_DATA, 0xFF);
    IO_WAIT();
    outb(PIC2_DATA, 0xFF);
    IO_WAIT();
}

void pic_mask(uint8_t irq)
{
    uint8_t value;
    uint16_t port;
 
    if (irq < 8)
        port = PIC1_DATA;
    else
    {
        port = PIC2_DATA;
        irq -= 8;
    }

    value = inb(port) | (1 << irq);
    outb(port, value);
}

void pic_unmask(uint8_t irq)
{
    uint8_t value;
    uint16_t port;
 
    if (irq < 8)
        port = PIC1_DATA;
    else
    {
        port = PIC2_DATA;
        irq -= 8;
    }

    value = inb(port) & ~(1 << irq);
    outb(port, value);}

void pic_send_eoi(const uint8_t irq)
{
    if (irq >= 8)
        outb(PIC2_COMMAND, PIC_EOI);
    
    outb(PIC1_COMMAND, PIC_EOI);
}