#pragma once

#include <stdint.h>
#include <stdbool.h>

#define NoKey           (-1)
#define CAPS_KEY        0x58
#define RELEASED_KEY    0xF0
#define RIGHT_SHIFT     0x12
#define LEFT_SHIFT      0x59
#define BACKSPACE       0x66
#define BACKTICK        0x0E
#define TAB             0x0D
#define INVALID_KEY     0xFF

/**
 * @brief Translate a scan code to a character.
 * 
 * @param code Scan code of the character.
 * @param isCaps Is the character in caps mode.
 * @param isShift Is the character in shift mode.
 * @return Character pressed.
*/
char translate(const uint8_t code, const bool isCaps, const bool isShift);