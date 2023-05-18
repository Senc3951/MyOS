#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define HEIGHT 25
#define WIDTH 80
#define TAB_LENGTH 4

/**
 * @brief Colors that can be printed.
*/
enum vga_color
{
	BLACK = 0,
	BLUE,
	GREEN,
	CYAN,
	RED,
	MAGENTA,
	BROWN,
	LIGHT_GREY,
	DARK_GREY,
	LIGHT_BLUE,
	LIGHT_GREEN,
	LIGHT_CYAN,
	LIGHT_RED,
	LIGHT_MAGENTA,
	LIGHT_BROWN,
	WHITE,
};

/**
 * @brief Clear the screen with a specific background color.
 * 
 * @param background Background color.
*/
void vga_clrcsn(const enum vga_color background);

/**
 * @brief Clear the screen.
*/
void vga_clrsn();

/**
 * @brief Set the colors of the screen.
 * 
 * @param background Background color of the screen.
 * @param foreground Foreground color of the screen.
*/
void vga_setcsn(const enum vga_color background, const enum vga_color foreground);

/**
 * @brief Print a character to the screen.
 * 
 * @param c Character to print.
*/
void vga_putc(const uint8_t c);

/**
 * @brief Print a string to the screen.
 * 
 * @param str String to print.
*/
void vga_puts(const uint8_t *str);