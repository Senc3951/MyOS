#include <kernel/io/io.h>
#include <kernel/arch/pic.h>

static bool g_AutoEOI = false;

static inline uint16_t getIRQRegister(const uint32_t ocw3)
{
    outb(PIC1_COMMAND, ocw3);
    outb(PIC2_COMMAND, ocw3);
    
    return (uint16_t)inb(PIC1_COMMAND) | (uint16_t)(inb(PIC2_COMMAND) << 8);
}

void PIC_Initialize(const uint8_t offsetPIC1, const uint8_t offsetPIC2, const bool autoEOI)
{
    // Initialize Control Word 1
    outb(PIC1_COMMAND, PIC_ICW1_ICW4 | PIC_ICW1_INITIALIZE);
    iowait();
    outb(PIC2_COMMAND, PIC_ICW1_ICW4 | PIC_ICW1_INITIALIZE);
    iowait();
    
    // Initialize Control Word 2
    outb(PIC1_DATA, offsetPIC1);
    iowait();
    outb(PIC2_DATA, offsetPIC2);
    iowait();
    
    // Initialize Control Word 3
    outb(PIC1_DATA, 4);    // Tell the Master that there is a Slave at IRQ2
    iowait();
    outb(PIC2_DATA, 2);
    iowait();
    
    uint8_t icw4 = PIC_ICW4_8086;
    if (autoEOI)
        icw4 |= PIC_ICW4_AUTO_EOI;
    
    // Initialize Control Word 4
    outb(PIC1_DATA, icw4);
    iowait();
    outb(PIC2_DATA, icw4);
    iowait();

    PIC_Disable();
}

void PIC_Disable()
{
    outb(PIC1_DATA, 0xFF);
    iowait();
    outb(PIC2_DATA, 0xFF);
    iowait();
}

void PIC_Mask(uint8_t irq)
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

void PIC_Unmask(uint8_t irq)
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

void PIC_SendEOI(const uint8_t irq)
{
    if (irq >= 8)
        outb(PIC2_COMMAND, PIC_EOI);
    
    outb(PIC1_COMMAND, PIC_EOI);
}

uint16_t PIC_GetIRR()
{
    return getIRQRegister(PIC_READ_IRR);

}

uint16_t PIC_GetISR()
{
    return getIRQRegister(PIC_READ_ISR);
}