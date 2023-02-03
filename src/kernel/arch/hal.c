#include <kernel/dev/pit.h>
#include <kernel/arch/hal.h>
#include <kernel/arch/gdt.h>
#include <kernel/arch/pic.h>
#include <kernel/arch/irq.h>
#include <kernel/arch/idt.h>
#include <kernel/sys/panic.h>
#include <kernel/sys/logger.h>
#include <kernel/dev/keyboard/keyboard.h>

static void initializeDevices();

void HAL_Initialize()
{    
    GDT_Initialize();
    LOGI("HAL", "GDT Initialized.\n");
    IDT_Initialize();
    LOGI("HAL", "IDT Initialized.\n");
    initializeDevices();
    LOGI("HAL", "Hardware Devices Initialized.\n");
    
    __STI();
}

void initializeDevices()
{
    PIC_Initialize(IRQ0, IRQ0 + 8, false);
    LOGI("HAL", "PIC Initialized.\n");
    
    PIT_Initialize(PIT_DEFAULT_FREQUENCY);
    LOGI("HAL", "PIT Initialized.\n");
    
    if (keyboard_Initialize())
        LOGI("HAL", "Keyboard Initialized.\n");
    else
        KPANIC("Failed initializing the Keyboard.\n");
}