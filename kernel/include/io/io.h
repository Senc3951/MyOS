#pragma once

#include <common.h>

void outb(const uint16_t port, const uint8_t value);
void outw(const uint16_t port, const uint16_t value);
void outl(const uint16_t port, const uint32_t value);
void outsm(const uint16_t port, uint8_t *data, uint32_t size);

uint8_t inb(const uint16_t port);
uint16_t inw(const uint16_t port);
uint32_t inl(const uint16_t port);
void insm(const uint16_t port, uint8_t *data, uint32_t size);

#define __IO_WAIT() { asm volatile("outb %%al, $0x80" : : "a"(0)); }
#define __CLI()     { asm volatile("cli" ::: "memory"); }
#define __STI()     { asm volatile("sti" ::: "memory"); }
#define __HALT()    { asm volatile("hlt" ::: "memory"); }
#define __PAUSE()   { asm volatile("pause" ::: "memory"); }

#define __HCF() { \
    __CLI();        \
    while (1)       \
        __HALT();   \
}
