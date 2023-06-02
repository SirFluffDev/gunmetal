#include "keyboard.h"

#include "idt.h"
#include "vga.h"

// Table that correlates scancodes to characters
static const u8 char_table[2][128] = {
    {
        0, // ???

        0, // Escape
        '1',
        '2',
        '3',
        '4',
        '5',
        '6',
        '7',
        '8',
        '9',
        '0',
        '-',
        '=',
        0, // Backspace
        0, // Tab
        'q',
        'w',
        'e',
        'r',
        't',
        'y',
        'u',
        'i',
        'o',
        'p',
        '[',
        ']',
        0, // Enter
        0, // Ctrl
        'a',
        's',
        'd',
        'f',
        'g',
        'h',
        'j',
        'k',
        'l',
        ';',
        '\'',
        '`',
        0, // Shift (left)
        '\\',
        'z',
        'x',
        'c',
        'v',
        'b',
        'n',
        'm',
        ',',
        '.',
        '/',
        0, // Shift (right)
        0, // Print Screen
        0, // Alt
        ' ',
        0, // Caps Lock
        0, // F1
        0, // F2
        0, // F3
        0, // F4
        0, // F5
        0, // F6
        0, // F7
        0, // F8
        0, // F9
        0, // F10
        0, // Pause break
        0, // Scroll lock
        0, // Home
        0, // Up arrow
        0, // Page up
        '-', // (Numpad)
        0, // Left arrow
        '5', // (Numpad)
        0, // Right arrow
        '+', // (Numpad)
        0, // End
        0, // Down Arrow
        0, // Page down
        0, // Insert
        0, // Delete

        0, // No idea
        0, // No idea
        0, // No idea

        0, // F11
        0, // F12
    },
    {
        0, // ???

        0, // Escape
        '!',
        '@',
        '#',
        '$',
        '%',
        '^',
        '&',
        '*',
        '(',
        ')',
        '_',
        '+',
        0, // Backspace
        0, // Tab
        'Q',
        'W',
        'E',
        'R',
        'T',
        'Y',
        'U',
        'I',
        'O',
        'P',
        '{',
        '}',
        0, // Enter
        0, // Ctrl
        'A',
        'S',
        'D',
        'F',
        'G',
        'H',
        'J',
        'K',
        'L',
        ':',
        '\"',
        '~',
        0, // Shift (left)
        '|',
        'Z',
        'X',
        'C',
        'V',
        'B',
        'N',
        'M',
        '<',
        '>',
        '?',
        0, // Shift (right)
        0, // Print Screen
        0, // Alt
        ' ',
        0, // Caps Lock
        0, // F1
        0, // F2
        0, // F3
        0, // F4
        0, // F5
        0, // F6
        0, // F7
        0, // F8
        0, // F9
        0, // F10
        0, // Pause break
        0, // Scroll lock
        0, // Home
        0, // Up arrow
        0, // Page up
        '-', // (Numpad)
        0, // Left arrow
        '5', // (Numpad)
        0, // Right arrow
        '+', // (Numpad)
        0, // End
        0, // Down Arrow
        0, // Page down
        0, // Insert
        0, // Delete

        0, // No idea
        0, // No idea
        0, // No idea

        0, // F11
        0, // F12
    },
};

// Keep track of keys
static bool keys_down[128];
static bool shift_is_down = 0;

static void keyboard_callback(registers_t regs) {
    // Get keyboard key
    u8 scancode = port_byte_in(0x60);

    // Check whether the key has been pressed or released
    u8 key_is_down = scancode < 0x80;
    if (!key_is_down) scancode -= 0x80;

    // Update whether the shift key is down
    if (scancode == LEFT_SHIFT || scancode == RIGHT_SHIFT) shift_is_down = key_is_down;

    // Save key state
    keys_down[scancode] = key_is_down;
}

bool key_is_down(u8 scancode) {
    return keys_down[scancode];
}

void init_keyboard() {
    register_interrupt_handler(IRQ1, &keyboard_callback);
}