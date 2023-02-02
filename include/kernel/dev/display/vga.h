#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdbool.h>

#define HEIGHT 25
#define WIDTH 80
#define TAB_LENGTH 4

/**
 * @brief Colors that can be printed.
*/
typedef enum VGAColor
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
} VGAColor_t;

/**
 * @brief enable the cursor.
*/
void VGA_EnableCursor();

/**
 * @brief Disable the cursor.
*/
void VGA_DisableCursor();

/**
 * @brief Clear the screen with a specific background color.
 * 
 * @param background Background color.
*/
void clrscnc(const VGAColor_t background);

/**
 * @brief Clear the screen.
*/
void clrscn();

/**
 * @brief Set the Colors of the screen.
 * 
 * @param background Background color of the screen.
 * @param foreground Foreground color of the screen.
*/
void setscnc(const VGAColor_t background, const VGAColor_t foreground);

/**
 * @brief Delete a character from the screen.
*/
void delc();

/**
 * @brief Print a character to the screen.
 * 
 * @param c Character to print.
*/
void putc(const uint8_t c);

/**
 * @brief Print a string to the screen.
 * 
 * @param str String to print.
*/
void puts(const uint8_t *str);

/**
 * @brief Print formatted string with arguments.
 * 
 * @param fmt Format to print.
 * @param args Arguments to print.
 * @return 1 if executed successfully, -1 otherwise.
*/
int vkprintf(const char *fmt, va_list args);

/**
 * @brief Print a formatted string.
 * 
 * @param fmt Format to print.
 * @return 1 if executed successfully, -1 otherwise.
*/
int kprintf(const char* fmt, ...);