#include <kernel/timer.h>
#include <kernel/isr.h>
#include <kernel/port.h>
#include <lib/stdio.h>
#include <sched/task.h>

#define PIT_CLOCK_FREQUENCY 1193180

static uint32_t tick = 0;

/* We don't want GCC complaining if we don't use the registers parameter. */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

static void _timer_callback(struct registers registers) {
	tick++;
	context_switch();
}

#pragma GCC diagnostic pop /* ignored "-Wunused-parameter" */

void init_timer(uint32_t frequency)
{
	uint32_t divisor;

	timer_debug("\n");

	/* Register our timer callback. */
	register_interrupt_handler(IRQ0, (isr_t)&_timer_callback);

	/* Get the 16-bit divisor. */
	divisor = PIT_CLOCK_FREQUENCY / frequency;

	/* Send the command byte. */
	PIT_COMMAND_OUT(0x36);

	/* The divisor must be sent byte-wise. */
	PIT_0_DATA_OUT((uint8_t)(divisor & 0xFF));
	PIT_0_DATA_OUT((uint8_t)((divisor>>8) & 0xFF));
}
