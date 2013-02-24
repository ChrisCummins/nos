#ifndef _STRING_H
#define _STRING_H

#include <nos/types.h>

#define is_digit(c) ((c) >= '0' && (c) <= '9')

size_t strlen(const char *string);
void *memory_copy(void *destination, uint32_t length, const uint8_t *src);
void *memory_set (void *destination, uint32_t length, uint8_t value);

#endif /* _STRING_H */
