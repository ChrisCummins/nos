#include <kernel/panic.h>

#include <lib/stdio.h>

#define DISABLE_INTERRUPTS __asm volatile("cli")
#define CPU_HALT for(;;)

extern void panic_halt(const char *message, const char *file, uint32_t line)
{
	DISABLE_INTERRUPTS;
	printf("\nPANIC(%s) at %s:%d\n", message, file, line);
	CPU_HALT;
}

extern void panic_assert(const char *file, uint32_t line, const char *assertion)
{
	DISABLE_INTERRUPTS;
	printf("\nASSERTION-FAILED(%s) at %s:%d\n", assertion, file, line);
	CPU_HALT;
}
