#include <io/io.h>
#include <sys/hal.h>
#include <dev/pit.h>
#include <arch/gdt.h>
#include <arch/idt.h>
#include <arch/pic.h>
#include <arch/irq.h>
#include <sys/logger.h>

static void devices_init()
{
    pit_init(PIT_DEFAULT_FREQUENCY);
    LOG("PIT initialized\n");
}

void hal_init()
{
    gdt_init();
    LOG("GDT initialized\n");
    idt_init();
    LOG("IDT initialized\n");
    pic_init(IRQ0, IRQ0 + 8, false);
    LOG("PIC initialized\n");
    
    STI();

    devices_init();
}