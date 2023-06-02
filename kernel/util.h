#ifndef TYPES_H
#define TYPES_H

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef char i8;
typedef short i16;
typedef int i32;
typedef long long i64;
typedef float f32;
typedef double f64;

typedef u8 bool;
#define true (1)
#define false (0)

#define CLI() asm("cli")
#define STI() asm("sti")

static inline u8 port_byte_in(u16 port) {
    u8 result;
    asm("inb %1, %0" : "=a" (result) : "dN" (port));
    return result;
}

static inline void port_byte_out(u16 port, u8 data) {
    asm("outb %1, %0" : : "dN" (port), "a" (data));
}

static inline void memset(void *dst, u8 value, u32 n) {
    u8 *d = dst;

    while (n-- > 0) {
        *d++ = value;
    }
}

static inline void memcpy(void *dst, const void *src, u32 n) {
    u8 *d = dst;
    const u8 *s = src;

    while (n-- > 0) {
        *d++ = *s++;
    }
}

#endif