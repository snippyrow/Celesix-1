#include "stdint.h"


extern "C" void isr0();
extern "C" void isr8();
extern "C" void irqkbd();

const char *exceptions[] = {
    "[proc. error]: Division by zero",
    "[proc. error]: Single-step interrupt (Debug)",
    "[proc. error]: NMI? What even is this.",
    "[proc. error]: Breakpoint",
    "[proc. error]: Overflow",
    "[proc. error]: Bound Range Exceeded",
    "[proc. error]: Invalid Opcode",
    "[proc. error]: Coprocessor not available",
    "[proc. error]: Double fault",
    "[proc. error]: Coprocessor Segment Overrun",
    "[proc. error]: Invalid Task State Segment",
    "[proc. error]: Segment not present",
    "[proc. error]: Stack Segment Fault",
    "[proc. error]: General Protection Fault..",
    "[proc. error]: Page Fault",
    "[proc. error]: ???",
    "[proc. error]: x87 Floating Point Exception",
    "[proc. error]: Alignment Check",
    "[proc. error]: Machine Check",
    "[proc. error]: SIMD Floating Point Exception",
    "[proc. error]: Virtualization Exception",
    "[proc. error]: Control Protection Exception",
    "[proc. error]: ???",
};

typedef struct {
    uint16_t low_offset;
    uint16_t selector;
    uint8_t always0;
    uint8_t flags;
    uint16_t high_offset;
} __attribute__((packed)) idt_gate_t;

typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idt_descriptor;

idt_descriptor idt_reg;
idt_gate_t idt[256];