#ifndef _UTIL_H
#define _UTIL_H

#include <nos/types.h>

#define _INTSIZEOF(n)    ((sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1))

/* Byte */
#define high_nibble(n) ((n) << 4)
#define low_nibble(n) ((n) & 0x0F)

/* Word */
#define high_byte(n) ((n) << 8)
#define low_byte(n) ((n) & 0x00FF)

#define min(x, y) (((x) < (y)) ? (x) : (y))
#define max(x, y) (((x) > (y)) ? (x) : (y))

/* Variable argument list functions. */
typedef char *va_list;
#define va_start(ap, last) (ap = (va_list) &last + _INTSIZEOF(last))
#define va_arg(ap, type)   (*(type *)((ap += _INTSIZEOF(type)) - _INTSIZEOF(type)))
#define va_end(ap)         (ap = (va_list) 0)

#ifdef DEBUG
# define assert(assertion) ((assertion) ? (void)0 : panic_assert(__FILE__, __LINE__, #assertion))
#else
# define assert(assertion)
#endif /* DEBUG */

#define PANIC(message) panic(message, __FILE__, __LINE__);

extern void panic(const char *message, const char *file, uint32_t line);
extern void panic_assert(const char *file, uint32_t line, const char *desc);

#endif /* _UTIL_H */
