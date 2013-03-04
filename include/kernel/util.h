#ifndef _UTIL_H
#define _UTIL_H

#include <kernel/panic.h>
#include <kernel/types.h>

#define _INTSIZEOF(n) ((sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1))

/* Byte */
#define high_nibble(n) ((n) << 4)
#define low_nibble(n) ((n) & 0x0F)

/* Word */
#define high_byte(n) ((n) << 8)
#define low_byte(n) ((n) & 0x00FF)

#define min(x, y) (((x) < (y)) ? (x) : (y))
#define max(x, y) (((x) > (y)) ? (x) : (y))

#endif /* _UTIL_H */
