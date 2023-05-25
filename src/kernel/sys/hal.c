#include <io/io.h>
#include <io/pci.h>
#include <sys/hal.h>
#include <dev/pit.h>
#include <arch/gdt.h>
#include <arch/idt.h>
#include <arch/pic.h>
#include <arch/irq.h>
#include <arch/nmi.h>
#include <dev/kb/kb.h>
#include <sys/logger.h>

static void devices_init()
{
    pit_init(PIT_DEFAULT_FREQUENCY);
    LOGI("PIT Initialized");
    if (keyboard_init())
        LOGI("Keyboard Initialized");
    else
        return;
    
    pci_init();
    LOGI("PCI Enumerated");
}

void hal_init()
{
    gdt_init();
    LOGI("GDT Initialized");
    idt_init();
    LOGI("IDT Initialized");
    nmi_enable();
    LOGI("NMI Enabled");
    pic_init(IRQ0, IRQ0 + 8, false);
    LOGI("PIC Initialized");
    STI();
    
    devices_init();
}