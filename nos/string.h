#ifndef _STRING_H
#define _STRING_H

#include <nos/types.h>

#define is_digit(c) ((c) >= '0' && (c) <= '9')

size_t strlen(const char *string);

#endif /* _STRING_H */
