#pragma once

#include <common.h>

#define COLOR_RED       "\033[31m"
#define COLOR_YELLOW    "\033[33m"
#define COLOR_DEFAULT   "\033[39m"

void uart_init();
void uart_putc(char c);