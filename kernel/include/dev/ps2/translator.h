#pragma once

#include <common.h>

#define SHIFT       (1 << 0)
#define CTL         (1 << 1)
#define ALT         (1 << 2)

#define CAPSLOCK    (1 << 3)
#define NUMLOCK     (1 << 4)
#define SCROLLLOCK  (1 << 5)

#define E0ESC       (1 << 6)

// C('A') == Control-A
#define C(x)        (x - '@')

extern uint8_t _Shiftcode[256];
extern uint8_t _Togglecode[256];
extern uint8_t _Normalmap[256];
extern uint8_t _Shiftmap[256];
extern uint8_t _Ctlmap[256];