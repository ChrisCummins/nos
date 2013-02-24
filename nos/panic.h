#ifndef _PANIC_H
#define _PANIC_H

#include <nos/types.h>

extern void panic_halt(const char *message, const char *file, uint32_t line);

/* Cause a kernel panic. This is a fatal error which falts execution of the
 * kernel. */
#define panic(message) panic_halt(message, __FILE__, __LINE__);

#endif /* _PANIC_H */
