#ifndef _ASSERT_H
#define _ASSERT_H

#include <kernel/types.h>

extern void panic_assert(const char *file, uint32_t line, const char *expression);

/* If the DEBUG macro was not defined at the moment <assert.h> was last
 * included, the macro assert() generates no code, and hence does nothing at
 * all. Otherwise, the macro assert() prints an error message and causes a fatal
 * kernel panic if expression is false (i.e., compares equal to zero). */
#ifdef DEBUG
# define assert(expression) ((expression) ?                             \
                             (void)0 : panic_assert(__FILE__, __LINE__, #expression))
#else
# define assert(expression)
#endif /* DEBUG */

#endif /* _ASSERT_H */
