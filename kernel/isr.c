#include <kernel/isr.h>
#include <kernel/kstream.h>
#include <kernel/port.h>

isr_t interrupt_handlers[256];

static void _execute_handler(struct registers_s registers) {
	if (interrupt_handlers[registers.interrupt_number] != 0) {
		isr_t handler;

		handler = interrupt_handlers[registers.interrupt_number];
		handler(registers);
	}
}

/* Called from ./interrupts.s */
void isr_handler(struct registers_s registers)
{
	_execute_handler(registers);
}

/* Called from ./interrupts.s */
void irq_handler(struct registers_s registers)
{
	/* If the interrupt originated form the slave PIC, then an EOI signal
	 * must be sent to the slave PIC. */
	if (registers.interrupt_number >= 40) {
		PIC_SLAVE_COMMAND_OUT(0x20);
	}

	/* Send an EOI signal to the master PIC. N.b. this must be sent even if
	 * the interrupt originated from the slave PIC, due to the manner in
	 * which they are daisy-chained. */
	PIC_MASTER_COMMAND_OUT(0x20);

	_execute_handler(registers);
}

void register_interrupt_handler(uint8_t interrupt_number, isr_t handler) {
	k_debug("Register ISR: [%d, %p]",
		interrupt_number, (uint32_t) handler);

	interrupt_handlers[interrupt_number] = handler;
}
