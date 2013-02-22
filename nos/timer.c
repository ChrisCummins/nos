#include <nos/timer.h>
#include <nos/isr.h>
#include <nos/kstream.h>
#include <nos/port.h>

#define PIT_CLOCK_FREQUENCY 1193180

static uint32_t tick = 0;

/* TODO: static-ize this callback. */
void timer_callback(struct registers_s registers) {
  tick++;

  k_debug("Tick: %d", tick);
}

void init_timer(uint32_t frequency)
{
  uint32_t divisor;

  k_message("Initialising Timer");

  /* Register our timer callback. */
  register_interrupt_handler(IRQ0, (isr_t)&timer_callback);

  /* Get the 16-bit divisor. */
  divisor = PIT_CLOCK_FREQUENCY / frequency;

  /* Send the command byte. */
  PIT_COMMAND_OUT(0x36);

  /* The divisor must be sent byte-wise. */
  PIT_0_DATA_OUT((uint8_t)(divisor & 0xFF));
  PIT_0_DATA_OUT((uint8_t)((divisor>>8) & 0xFF));
}
