#ifndef _IDT_H
#define _IDT_H

#include <types.h>

struct idt_entry_s {
	uint16_t base_low;  /* The lower 16 bits of the interrupt address. */
	uint16_t selector;  /* Kernel segment selector. */
	uint8_t  always0;
	uint8_t  flags;
	uint16_t base_high; /* The upper 16 bits of the address to jump to. */
} __attribute__((packed));

struct idt_pointer_s {
	uint16_t limit;
	uint32_t base; /* The address of the idt_entry_s first element. */
} __attribute__((packed));

/* GDT access flag positions. */
#define IDT_FLAGS_P_SHIFT     7
#define IDT_FLAGS_DPL_SHIFT   5

void init_idt(void);

/*
 * 0     - Division by zero exception
 * 1     - Debug exception
 * 2     - Non maskable interrupt
 * 3     - Breakpoint exception
 * 4     - 'Into detected overflow'
 * 5     - Out of bounds exception
 * 6     - Invalid opcode exception
 * 7     - No coprocessor exception
 * 8     - Double fault (pushes an error code)
 * 9     - Coprocessor segment overrun
 * 10    - Bad TSS (pushes an error code)
 * 11    - Segment not present (pushes an error code)
 * 12    - Stack fault (pushes an error code)
 * 13    - General protection fault (pushes an error code)
 * 14    - Page fault (pushes an error code)
 * 15    - Unknown interrupt exception
 * 16    - Coprocessor fault
 * 17    - Alignment check exception
 * 18    - Machine check exception
 * 19-31 - Reserved
 */
extern void isr0 (void);
extern void isr1 (void);
extern void isr2 (void);
extern void isr3 (void);
extern void isr4 (void);
extern void isr5 (void);
extern void isr6 (void);
extern void isr7 (void);
extern void isr8 (void);
extern void isr9 (void);
extern void isr10(void);
extern void isr11(void);
extern void isr12(void);
extern void isr13(void);
extern void isr14(void);
extern void isr15(void);
extern void isr16(void);
extern void isr17(void);
extern void isr18(void);
extern void isr19(void);
extern void isr20(void);
extern void isr21(void);
extern void isr22(void);
extern void isr23(void);
extern void isr24(void);
extern void isr25(void);
extern void isr26(void);
extern void isr27(void);
extern void isr28(void);
extern void isr29(void);
extern void isr30(void);
extern void isr31(void);
extern void irq0 (void);
extern void irq1 (void);
extern void irq2 (void);
extern void irq3 (void);
extern void irq4 (void);
extern void irq5 (void);
extern void irq6 (void);
extern void irq7 (void);
extern void irq8 (void);
extern void irq9 (void);
extern void irq10(void);
extern void irq11(void);
extern void irq12(void);
extern void irq13(void);
extern void irq14(void);
extern void irq15(void);

#endif /* _IDT_H */
