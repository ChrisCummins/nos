#include <kernel/panic.h>

#include <kernel/kstream.h>

#define DISABLE_INTERRUPTS __asm volatile("cli")
#define CPU_HALT for(;;)

extern void panic_halt(const char *message, const char *file, uint32_t line)
{
	DISABLE_INTERRUPTS;

	k_critical("PANIC(%s) at %s:%d", message, file, line);

	CPU_HALT;
}

extern void panic_assert(const char *file, uint32_t line, const char *assertion)
{
	DISABLE_INTERRUPTS;

	k_critical("ASSERTION-FAILED(%s) at %s:%d", assertion, file, line);

	CPU_HALT;
}