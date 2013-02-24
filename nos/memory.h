#ifndef _MEMORY_H
#define _MEMORY_H

#include <nos/types.h>

void memory_copy(void *destination, uint32_t length, const uint8_t *src);
void memory_set (void *destination, uint32_t length, uint8_t value);

#endif /* _MEMORY_H */
