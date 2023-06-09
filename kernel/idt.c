#include "idt.h"

#include "vga.h"

// Loads IDT
extern void idt_flush(u32);

idt_entry_t idt_entries[256];
idt_ptr_t idt_ptr;

static void idt_set_gate(u8 num, u32 base, u16 sel, u8 flags) {
    idt_entries[num].base_lo = base & 0xFFFF;
    idt_entries[num].base_hi = (base >> 16) & 0xFFFF;

    idt_entries[num].sel = sel;
    idt_entries[num].always0 = 0;

    idt_entries[num].flags = flags;
}

// Set IDT gates and load
void init_idt() {
    idt_ptr.limit = sizeof(idt_entry_t) * 256 - 1;
    idt_ptr.base = (u32)&idt_entries;

    // Fill idt_entries with zero
    memset(&idt_entries, 0, sizeof(idt_entry_t)*256);

    // Set IDT gates for ISRs
    idt_set_gate( 0, (u32) isr0 , 0x08, 0x8e);
    idt_set_gate( 1, (u32) isr1 , 0x08, 0x8e);
    idt_set_gate( 2, (u32) isr2 , 0x08, 0x8e);
    idt_set_gate( 3, (u32) isr3 , 0x08, 0x8e);
    idt_set_gate( 4, (u32) isr4 , 0x08, 0x8e);
    idt_set_gate( 5, (u32) isr5 , 0x08, 0x8e);
    idt_set_gate( 6, (u32) isr6 , 0x08, 0x8e);
    idt_set_gate( 7, (u32) isr7 , 0x08, 0x8e);
    idt_set_gate( 8, (u32) isr8 , 0x08, 0x8e);
    idt_set_gate( 9, (u32) isr9 , 0x08, 0x8e);
    idt_set_gate(10, (u32) isr10, 0x08, 0x8e);
    idt_set_gate(11, (u32) isr11, 0x08, 0x8e);
    idt_set_gate(12, (u32) isr12, 0x08, 0x8e);
    idt_set_gate(13, (u32) isr13, 0x08, 0x8e);
    idt_set_gate(14, (u32) isr14, 0x08, 0x8e);
    idt_set_gate(15, (u32) isr15, 0x08, 0x8e);
    idt_set_gate(16, (u32) isr16, 0x08, 0x8e);
    idt_set_gate(17, (u32) isr17, 0x08, 0x8e);
    idt_set_gate(18, (u32) isr18, 0x08, 0x8e);
    idt_set_gate(19, (u32) isr19, 0x08, 0x8e);
    idt_set_gate(20, (u32) isr20, 0x08, 0x8e);
    idt_set_gate(21, (u32) isr21, 0x08, 0x8e);
    idt_set_gate(22, (u32) isr22, 0x08, 0x8e);
    idt_set_gate(23, (u32) isr23, 0x08, 0x8e);
    idt_set_gate(24, (u32) isr24, 0x08, 0x8e);
    idt_set_gate(25, (u32) isr25, 0x08, 0x8e);
    idt_set_gate(26, (u32) isr26, 0x08, 0x8e);
    idt_set_gate(27, (u32) isr27, 0x08, 0x8e);
    idt_set_gate(28, (u32) isr28, 0x08, 0x8e);
    idt_set_gate(29, (u32) isr29, 0x08, 0x8e);
    idt_set_gate(30, (u32) isr30, 0x08, 0x8e);
    idt_set_gate(31, (u32) isr31, 0x08, 0x8e);

    // Remap IRQ table (trust)
    port_byte_out(0x20, 0x11);
    port_byte_out(0xA0, 0x11);
    port_byte_out(0x21, 0x20);
    port_byte_out(0xA1, 0x28);
    port_byte_out(0x21, 0x04);
    port_byte_out(0xA1, 0x02);
    port_byte_out(0x21, 0x01);
    port_byte_out(0xA1, 0x01);
    port_byte_out(0x21, 0x0);
    port_byte_out(0xA1, 0x0);

    // Set IDT gates for IRQs
    idt_set_gate(32, (u32) irq0,  0x08, 0x8E);
    idt_set_gate(33, (u32) irq1,  0x08, 0x8E);
    idt_set_gate(34, (u32) irq2,  0x08, 0x8E);
    idt_set_gate(35, (u32) irq3,  0x08, 0x8E);
    idt_set_gate(36, (u32) irq4,  0x08, 0x8E);
    idt_set_gate(37, (u32) irq5,  0x08, 0x8E);
    idt_set_gate(38, (u32) irq6,  0x08, 0x8E);
    idt_set_gate(39, (u32) irq7,  0x08, 0x8E);
    idt_set_gate(40, (u32) irq8,  0x08, 0x8E);
    idt_set_gate(41, (u32) irq9,  0x08, 0x8E);
    idt_set_gate(42, (u32) irq10, 0x08, 0x8E);
    idt_set_gate(43, (u32) irq11, 0x08, 0x8E);
    idt_set_gate(44, (u32) irq12, 0x08, 0x8E);
    idt_set_gate(45, (u32) irq13, 0x08, 0x8E);
    idt_set_gate(46, (u32) irq14, 0x08, 0x8E);
    idt_set_gate(47, (u32) irq15, 0x08, 0x8E);

    // Set IDT
    idt_flush((u32)&idt_ptr);
}

// ISRs

static const u8 *exceptions[32] = {
    // Exceptions we need to worry about
    "Division by zero",
    "Debug",
    "Non-maskable interrupt",
    "Breakpoint",
    "Overflow",
    "OOB",
    "Invalid opcode",
    "No coprocessor",
    "Double fault",
    "Coprocessor segment overrun",
    "Bad TSS",
    "Segment not present",
    "Stack fault",
    "General protection fault",
    "Page fault",
    "Unrecognized interrupt",
    "Coprocessor fault",
    "Alignment check",
    "Machine check",

    // Reserved
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
};

void isr_handler(registers_t regs) {
    vga_print("Interrupt:", 3, 2, 0, 24);
    vga_print(exceptions[regs.int_no], 3, 2, 11, 24);

    // Hang
    while (true) {}
}

// IRQ Callback Table
static isr_t interrupt_handlers[256];

static void irq_clear_mask(u32 i) {
    u16 port = i < 8 ? PIC1_DATA : PIC2_DATA;
    u8 value = port_byte_in(port) & ~(1 << i);
    port_byte_out(port, value);
}

void register_interrupt_handler(u32 irq_id, isr_t handler) {
    CLI();
    
    irq_clear_mask(irq_id);
    interrupt_handlers[irq_id] = handler;

    STI();
}

void irq_handler(registers_t regs) {
    // Send an EOI (end of interrupt) signal to the PICs.
    // If this interrupt involved the slave.
    if (regs.int_no >= IRQ8) port_byte_out(0xA0, 0x20);

    // Send reset signal to master.
    port_byte_out(0x20, 0x20);

    isr_t handler = interrupt_handlers[regs.int_no];
    handler(regs);
}