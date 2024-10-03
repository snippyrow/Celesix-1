#include "idt.h"
#include "../Util.h"
#include "stdint.h"


void set_idt_gate(int n, uint32_t handler) {
    idt[n].low_offset = low_16(handler);
    idt[n].selector = 0x08; // see GDT (CODE_SEG)
    idt[n].always0 = 0;
    // 0x8E = 1  00 0 1  110
    //        P DPL 0 D Type
    idt[n].flags = 0x8E;
    idt[n].high_offset = high_16(handler);
}

void irq_ghandler() {
    // General handler for processor exceptions
    print("[proc. error]: General Interrupt Test\n");
}

extern "C" void isr_ghandler(int int_no, int err_code) { // arguments already on the stack from processor
    // General handler for processor exceptions
    print(exceptions[int_no]);
    print(" (");
    printd(int_no);
    print(")\n");
    return;
}

void isr_install() {
    set_idt_gate(0,(int)isr0);

    set_idt_gate(8,(int)isr8);

    // IRQ ISRs (primary PIC)
    set_idt_gate(32, (uint32_t)irq_ghandler); // This is the system timer and MUST be enabled, or else we receive double/tripple faults
    // ...
    set_idt_gate(39, (uint32_t)irq_ghandler);

    // IRQ ISRs (secondary PIC)
    set_idt_gate(40, (uint32_t)irq_ghandler);
    // ...
    set_idt_gate(47, (uint32_t)irq_ghandler);


    set_idt_gate(33, (uint32_t)irqkbd); // keyboard
    /*
    for (int i=1;i<22;i++) {
        set_idt_gate(i,(uint32_t)isr_ghandler);
    }
    */
    outb(0x20, 0x11);
    outb(0xA0, 0x11);

    // ICW2
    outb(0x21, 0x20);
    outb(0xA1, 0x28);

    // ICW3
    outb(0x21, 0x04);
    outb(0xA1, 0x02);

    // ICW4
    outb(0x21, 0x01);
    outb(0xA1, 0x01);

    // OCW1
    outb(0x21, 0x0);
    outb(0xA1, 0x0);

    outb(0x21, 0xFD);

    idt_reg.base = (uint32_t) &idt;
    idt_reg.limit = 0xff * sizeof(idt_gate_t) - 1;
    asm volatile("lidt (%0)" : : "r" (&idt_reg));
}