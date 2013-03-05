#include <kernel/idt.h>

#include <kernel/isr.h>
#include <kernel/kstream.h>
#include <kernel/util.h>
#include <kernel/port.h>
#include <lib/string.h>

extern void idt_flush(uint32_t);

/* Defined in ./isr.c */
extern isr_t interrupt_handlers[];

static void idt_set_gate(uint8_t number, uint32_t base,
                         uint16_t selector, uint8_t flags);

static struct idt_entry   idt_entries[256];
static struct idt_pointer idt_p;

void init_idt()
{
	k_message("Initialising IDT\n");

	idt_p.limit = sizeof(struct idt_entry) * 256 - 1;
	idt_p.base  = (uint32_t)&idt_entries;

	memset((void *)&idt_entries, 0x0, (sizeof(struct idt_entry)) * 256);
	memset((void *)&interrupt_handlers, 0x0, (sizeof(isr_t)) * 256);

	/* When the computer boots, the default interrupt mappings are:
	 *    IRQ 0..7 - INT 0x8..0xF
	 *    IRQ 8..15 - INT 0x70..0x77
	 *
	 * This causes us somewhat of a problem. The master's IRQ mappings
	 * (0x8-0xF) conflict with the interrupt numbers used by the CPU to
	 * signal exceptions and faults. The normal thing to do is to remap the
	 * PICs so that IRQs 0..15 correspond to ISRs 32..47 (31 being the last
	 * CPU-used ISR). */
	PIC_MASTER_COMMAND_OUT (0x11);
	PIC_SLAVE_COMMAND_OUT  (0x11);
	PIC_MASTER_DATA_OUT    (0x20);
	PIC_SLAVE_DATA_OUT     (0x28);
	PIC_MASTER_DATA_OUT    (0x04);
	PIC_SLAVE_DATA_OUT     (0x02);
	PIC_MASTER_DATA_OUT    (0x01);
	PIC_SLAVE_DATA_OUT     (0x01);
	PIC_MASTER_DATA_OUT    (0x00);
	PIC_SLAVE_DATA_OUT     (0x00);

	idt_set_gate( 0, (uint32_t)isr0 , 0x08, 0x8E);
	idt_set_gate( 1, (uint32_t)isr1 , 0x08, 0x8E);
	idt_set_gate( 2, (uint32_t)isr2 , 0x08, 0x8E);
	idt_set_gate( 3, (uint32_t)isr3 , 0x08, 0x8E);
	idt_set_gate( 4, (uint32_t)isr4 , 0x08, 0x8E);
	idt_set_gate( 5, (uint32_t)isr5 , 0x08, 0x8E);
	idt_set_gate( 6, (uint32_t)isr6 , 0x08, 0x8E);
	idt_set_gate( 7, (uint32_t)isr7 , 0x08, 0x8E);
	idt_set_gate( 8, (uint32_t)isr8 , 0x08, 0x8E);
	idt_set_gate( 9, (uint32_t)isr9 , 0x08, 0x8E);
	idt_set_gate(10, (uint32_t)isr10, 0x08, 0x8E);
	idt_set_gate(11, (uint32_t)isr11, 0x08, 0x8E);
	idt_set_gate(12, (uint32_t)isr12, 0x08, 0x8E);
	idt_set_gate(13, (uint32_t)isr13, 0x08, 0x8E);
	idt_set_gate(14, (uint32_t)isr14, 0x08, 0x8E);
	idt_set_gate(15, (uint32_t)isr15, 0x08, 0x8E);
	idt_set_gate(16, (uint32_t)isr16, 0x08, 0x8E);
	idt_set_gate(17, (uint32_t)isr17, 0x08, 0x8E);
	idt_set_gate(18, (uint32_t)isr18, 0x08, 0x8E);
	idt_set_gate(19, (uint32_t)isr19, 0x08, 0x8E);
	idt_set_gate(20, (uint32_t)isr20, 0x08, 0x8E);
	idt_set_gate(21, (uint32_t)isr21, 0x08, 0x8E);
	idt_set_gate(22, (uint32_t)isr22, 0x08, 0x8E);
	idt_set_gate(23, (uint32_t)isr23, 0x08, 0x8E);
	idt_set_gate(24, (uint32_t)isr24, 0x08, 0x8E);
	idt_set_gate(25, (uint32_t)isr25, 0x08, 0x8E);
	idt_set_gate(26, (uint32_t)isr26, 0x08, 0x8E);
	idt_set_gate(27, (uint32_t)isr27, 0x08, 0x8E);
	idt_set_gate(28, (uint32_t)isr28, 0x08, 0x8E);
	idt_set_gate(29, (uint32_t)isr29, 0x08, 0x8E);
	idt_set_gate(30, (uint32_t)isr30, 0x08, 0x8E);
	idt_set_gate(31, (uint32_t)isr31, 0x08, 0x8E);
	idt_set_gate(32, (uint32_t)irq0,  0x08, 0x8E);
	idt_set_gate(33, (uint32_t)irq1,  0x08, 0x8E);
	idt_set_gate(34, (uint32_t)irq2,  0x08, 0x8E);
	idt_set_gate(35, (uint32_t)irq3,  0x08, 0x8E);
	idt_set_gate(36, (uint32_t)irq4,  0x08, 0x8E);
	idt_set_gate(37, (uint32_t)irq5,  0x08, 0x8E);
	idt_set_gate(38, (uint32_t)irq6,  0x08, 0x8E);
	idt_set_gate(39, (uint32_t)irq7,  0x08, 0x8E);
	idt_set_gate(40, (uint32_t)irq8,  0x08, 0x8E);
	idt_set_gate(41, (uint32_t)irq9,  0x08, 0x8E);
	idt_set_gate(42, (uint32_t)irq10, 0x08, 0x8E);
	idt_set_gate(43, (uint32_t)irq11, 0x08, 0x8E);
	idt_set_gate(44, (uint32_t)irq12, 0x08, 0x8E);
	idt_set_gate(45, (uint32_t)irq13, 0x08, 0x8E);
	idt_set_gate(46, (uint32_t)irq14, 0x08, 0x8E);
	idt_set_gate(47, (uint32_t)irq15, 0x08, 0x8E);

	idt_flush((uint32_t)&idt_p);
}

static void idt_set_gate(uint8_t number, uint32_t base,
                         uint16_t selector, uint8_t flags)
{
	idt_entries[number].base_low  = base         & 0xFFFF;
	idt_entries[number].base_high = (base >> 16) & 0xFFFF;

	idt_entries[number].selector  = selector;
	idt_entries[number].always0   = 0;

	/* The OR below must be uncommented when we get to using user-mode.  It
	 * sets the interrupt gate's privilege level to 3. */
	idt_entries[number].flags     = flags /* | 0x60 */;
}
