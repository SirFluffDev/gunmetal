#include "vga.h"

#include "font.h"

// Used for double buffering
u8 vga_buffer[VGA_SIZE];

// Set a custom VGA palette
void configure_vga_palette(const color_t palette[], u8 number_of_colors) {
    port_byte_out(0x03C6, 0xff); // Mask all registers
    port_byte_out(0x03C8, 0x00); // Start from first palette color

    for (u8 i = 0; i < number_of_colors; i++) {
        color_t color = palette[i];

        port_byte_out(0x03C9, color.r >> 2);
        port_byte_out(0x03C9, color.g >> 2);
        port_byte_out(0x03C9, color.b >> 2);
    }
}

// Print a single character to VGA memory
void vga_print_character(const u8 character, u8 fg_color, u8 bg_color, u16 x, u16 y) {
    u8* vga_memory = vga_buffer + (y * VGA_WIDTH + x);

    for (u8 y = 0; y < FONT_SIZE; y++) {
        u8 row = FONT[character][y];

        // Iterate through bits and set a pixel for every positive bit
        u8 last_bit = 0;
        for(u8 x = 0; x < FONT_SIZE; x++) {
            u32 index = x;

            if (row & 1) { 
                vga_memory[index] = fg_color; 
            }
            else if (last_bit) {
                vga_memory[index] = bg_color; 
            }

            last_bit = row & 1;
            row = row >> 1;
        }

        // Move on to next row
        vga_memory += VGA_WIDTH;
    }
}

// Print a byte in hex to VGA memory
void vga_print_byte(u8 byte, u16 x, u16 y) {
    u8 character_lo = 0;
    u8 byte_lo = byte << 4;

    if (byte_lo == 0x00) character_lo = '0';
    if (byte_lo == 0x10) character_lo = '1';
    if (byte_lo == 0x20) character_lo = '2';
    if (byte_lo == 0x30) character_lo = '3';
    if (byte_lo == 0x40) character_lo = '4';
    if (byte_lo == 0x50) character_lo = '5';
    if (byte_lo == 0x60) character_lo = '6';
    if (byte_lo == 0x70) character_lo = '7';
    if (byte_lo == 0x80) character_lo = '8';
    if (byte_lo == 0x90) character_lo = '9';
    if (byte_lo == 0xA0) character_lo = 'A';
    if (byte_lo == 0xB0) character_lo = 'B';
    if (byte_lo == 0xC0) character_lo = 'C';
    if (byte_lo == 0xD0) character_lo = 'D';
    if (byte_lo == 0xE0) character_lo = 'E';
    if (byte_lo == 0xF0) character_lo = 'F';
    
    u8 character_hi = 0;
    u8 byte_hi = byte >> 4;

    if (byte_hi == 0x00) character_hi = '0';
    if (byte_hi == 0x01) character_hi = '1';
    if (byte_hi == 0x02) character_hi = '2';
    if (byte_hi == 0x03) character_hi = '3';
    if (byte_hi == 0x04) character_hi = '4';
    if (byte_hi == 0x05) character_hi = '5';
    if (byte_hi == 0x06) character_hi = '6';
    if (byte_hi == 0x07) character_hi = '7';
    if (byte_hi == 0x08) character_hi = '8';
    if (byte_hi == 0x09) character_hi = '9';
    if (byte_hi == 0x0A) character_hi = 'A';
    if (byte_hi == 0x0B) character_hi = 'B';
    if (byte_hi == 0x0C) character_hi = 'C';
    if (byte_hi == 0x0D) character_hi = 'D';
    if (byte_hi == 0x0E) character_hi = 'E';
    if (byte_hi == 0x0F) character_hi = 'F';

    vga_print_character(character_hi, 12, 14, x,   y);
    vga_print_character(character_lo, 12, 14, x+8, y);
}

// Print a string to VGA memory
void vga_print(const u8 text[], u8 fg_color, u8 bg_color, u16 x, u16 y) {
    // Create a pointer to VGA memory
    u8* vga_memory = vga_buffer + (y * VGA_WIDTH + x);

    for (u8 y = 0; y < FONT_SIZE; y++) {
        for(u8 l = 0; text[l] != '\0'; l++) {
            // Get bits for current row of font character
            u8 row = FONT[text[l]][y];

            // If there's nothing to be drawn here, skip
            if (row == 0) continue;

            // Iterate through bits and set a pixel for every positive bit
            u8 last_bit = 0;
            for(u8 x = 0; x < FONT_SIZE; x++) {
                u32 index = l * FONT_SIZE + x;
                if (row & 1) { 
                    vga_memory[index] = fg_color; 
                }
                else if (last_bit) {
                    vga_memory[index] = bg_color; 
                }

                last_bit = row & 1;
                row = row >> 1;
            }
        }

        // Move on to next row
        vga_memory += VGA_WIDTH;
    }
}

// Copy the buffer to VGA memory
void swap() {
    memcpy((void *) VGA_ADDRESS, vga_buffer, VGA_SIZE);
}