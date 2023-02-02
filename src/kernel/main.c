#include <kernel/io/io.h>
#include <kernel/sys/logger.h>
#include <kernel/sys/panic.h>
void kmain(uintptr_t magic, void *mbTags)
{
    clrscn();
    kprintf("Hello WORLD\n");
    LOGI(false, "This is an info message.\n");
    LOGW(true, "This is a warning message.\n");
    LOGE(true, "This is an error message.\n");
    LOGCE(false, "This is a critical error message.\n");
    
    while (1)
        __HALT();
}