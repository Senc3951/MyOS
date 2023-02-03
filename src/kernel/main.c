#include <kernel/io/io.h>
#include <kernel/mem/mmu.h>
#include <kernel/arch/hal.h>
#include <kernel/dev/display/vga.h>

#include <kernel/dev/pit.h>
#include <kernel/dev/keyboard/keyboard.h>

void kmain(uintptr_t magic, MB2Info_t *mbTags)
{
    clrscn();
    __CLI();
    
    HAL_Initialize();
    MMU_Initialize(magic, mbTags);
    
    kprintf("SLEEPING FOR 3 SECONDS.\n");
    PIT_Sleep(3000);
    kprintf("PRINTING PRESSED CHARACTERS UNTIL GETTING AN ~\n");
    char c;
    while ((c = keyboard_Getc()) != '~')
        putc(c);
    kprintf("STOPPED\n");
    
    while (1)
        __HALT();
}