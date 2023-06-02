#include "timer.h"

#include "idt.h"
#include "vga.h"

#define PIT_A 0x40
#define PIT_B 0x41
#define PIT_C 0x42
#define PIT_CONTROL 0x43

#define PIT_MASK 0xFF
#define PIT_SET 0x36

#define PIT_HZ 1193181

static struct {
    u32 frequency;
    u32 divisor;
    u32 ticks;
} state;

static void timer_callback(registers_t regs) {
    state.ticks++;
}

void init_timer(u32 frequency) {
    u32 divisor = 1193181 / frequency;
    
    // Setup state
    state.frequency = frequency;
    state.divisor = divisor;
    state.ticks = 0;

    // Send new frequency to PIT
    port_byte_out(PIT_CONTROL, PIT_SET);
    port_byte_out(PIT_A, divisor & PIT_MASK);
    port_byte_out(PIT_A, (divisor >> 8) & PIT_MASK);

    // Connect timer callback to PIT irq
    register_interrupt_handler(IRQ0, &timer_callback);
}

u32 timer_get() {
    return state.ticks;
}