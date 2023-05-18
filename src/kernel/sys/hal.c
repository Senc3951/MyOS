#include <io/io.h>
#include <sys/hal.h>
#include <arch/gdt.h>
#include <arch/idt.h>
#include <sys/logger.h>

void hal_init()
{
    gdt_init();
    LOG("GDT initialized\n");
    idt_init();
    LOG("IDT initialized\n");
    
    STI();
}