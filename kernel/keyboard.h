#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "util.h"

enum KEYS {
    BACKSPACE = 0x0E,
    ENTER = 0x1C,

    LEFT_SHIFT  = 0x2A,
    RIGHT_SHIFT = 0x36,

    KEY_A = 0x1E,
    KEY_W = 0x11,
    KEY_S = 0x1F,
    KEY_D = 0x20,

    ARROW_LEFT  = 0x4B,
    ARROW_UP    = 0x48,
    ARROW_DOWN  = 0x50,
    ARROW_RIGHT = 0x4D,
};

bool key_is_down(u8 scancode);
void init_keyboard();

#endif