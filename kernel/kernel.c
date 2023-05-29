#include "types.h"

#include "io.h"
#include "font.h"

#define VGA_ADDRESS 0xA0000

#define VGA_WIDTH 320
#define VGA_HEIGHT 200

union Color
{
    u32 hex;
    struct { u8 b, g, r; };
};

void configure_vga_palette(const union Color palette[], u8 number_of_colors) {
    port_byte_out(0x03C6, 0xff); // Mask all registers
    port_byte_out(0x03C8, 0x00); // Start from first palette color

    for (u8 i = 0; i < number_of_colors; i++) {
        union Color color = palette[i];

        port_byte_out(0x03C9, color.r >> 2);
        port_byte_out(0x03C9, color.g >> 2);
        port_byte_out(0x03C9, color.b >> 2);
    }
}

#define TOTAL_COLORS 16
const union Color palette[TOTAL_COLORS] = {
    0x1a1c2c, // 00 - Black
    0x5d275d, // 01 - Dark red
    0xb13e53, // 02 - Red
    0xef7d57, // 03 - Orange
    0xffcd75, // 04 - Yellow
    0xa7f070, // 05 - Light green
    0x38b764, // 06 - Green
    0x257179, // 07 - Dark green
    0x29366f, // 08 - Dark blue
    0x3b5dc9, // 09 - Blue
    0x41a6f6, // 10 - Light blue
    0x73eff7, // 11 - Lighter blue
    0xf4f4f4, // 12 - White
    0x94b0c2, // 13 - Light gray
    0x566c86, // 14 - Gray
    0x333c57, // 15 - Dark gray
};

void vga_print(u8 text[], u8 fg_color, u8 bg_color, u16 x, u16 y) {
    // Create a pointer to VGA memory
    u8* vga_memory = (u8*) VGA_ADDRESS + (y * VGA_WIDTH + x);

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

void _start() {
    // Load defined palette onto VGA hardware
    configure_vga_palette(palette, TOTAL_COLORS);

    // Cool palette test background
    u8* vga_memory = (u8*) VGA_ADDRESS;
    for (u16 y = 0; y < VGA_HEIGHT; y++) {
        for (u16 x = 0; x < VGA_WIDTH; x++) {
            vga_memory[x] = (x+y)/8 % TOTAL_COLORS;
        }

        vga_memory += VGA_WIDTH;
    } 

    // Write some cool text
    vga_print("Hello, kernel.", 12, 14, 0, 0);
    vga_print("Wahoo!", 5, 7, 15, 0);
}