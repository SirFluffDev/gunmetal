#ifndef VGA_H
#define VGA_H

#include "util.h"

#define VGA_ADDRESS 0xA0000

#define VGA_WIDTH 320
#define VGA_HEIGHT 200
#define VGA_SIZE (VGA_WIDTH * VGA_HEIGHT)

typedef union color_union
{
    u32 hex;
    struct { u8 b, g, r; };
} color_t;

u8 vga_buffer[VGA_SIZE];

void configure_vga_palette(const color_t palette[], u8 number_of_colors);
void vga_print_character(const u8 character, u8 fg_color, u8 bg_color, u16 x, u16 y);
void vga_print_byte(u8 byte, u16 x, u16 y);
void vga_print(const u8 text[], u8 fg_color, u8 bg_color, u16 x, u16 y);
void swap();

#endif