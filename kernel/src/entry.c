#include <boot/mb2.h>
#include <io/io.h>
#include <io/uart.h>
#include <assert.h>
#include <log.h>
#include <libc/string.h>
#include <mem/pmm.h>
#include <mem/vmm.h>
#include <arch/gdt.h>
#include <arch/idt.h>
#include <arch/pic.h>
#include <arch/rsdt.h>
#include <arch/madt.h>
#include <arch/lapic.h>
#include <arch/ioapic.h>
#include <dev/fb/screen.h>
#include <dev/ps2/kbd.h>
#include <dev/timer.h>

// Linker exports
extern uintptr_t __kernel_start__;
extern uintptr_t __kernel_end__;
extern uintptr_t __write_protect_start__;
extern uintptr_t __write_protect_end__;

const uintptr_t _KERNEL_START = (uintptr_t)&__kernel_start__;
const uintptr_t _KERNEL_END = (uintptr_t)&__kernel_end__;
const uintptr_t _KERNEL_WRITE_PROTECT_START = (uintptr_t)&__write_protect_start__;
const uintptr_t _KERNEL_WRITE_PROTECT_END = (uintptr_t)&__write_protect_end__;

__NO_RETURN__ void kentry(uint32_t magic, const struct MULTIBOOT_INFO *mb)
{
    // Initialize log
    uart_init();
    
    // Initialize multiboot parser
    mb2_init(magic, mb);
    
    LOG_WARNING_INFO("Kernel at %p-%p. Write protected region at %p-%p\n", _KERNEL_START, _KERNEL_END, _KERNEL_WRITE_PROTECT_START, _KERNEL_WRITE_PROTECT_END);
    LOG_WARNING_INFO("Bootloader: %s. Cmdline arguments: %s\n", mb2_bootloader_name(), mb2_cmdline());
    
    // Initialize memory
    pmm_init();
    vmm_init();
    
    // Initialize interrupts
    gdt_init();
    tss_init();
    idt_init();
    pic_init(IRQ0, IRQ0 + 8, false);
    
    // Initialize APIC
    rsdp_init();
    madt_init();
    lapic_init();
    ioapic_init();
    
    // Initialize devices
    screen_init();
    kbd_init();
    timer_init();
    
    __STI();
    LOG("FINISHED!\n");

    while (1);
    __HCF();
}