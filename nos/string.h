#ifndef _STRING_H
#define _STRING_H

#include <nos/types.h>

#define is_digit(c) ((c) >= '0' && (c) <= '9')

size_t strlen(const char *string);

/* The strcpy() function copies the string pointed to by src, including the
 * terminating null byte ('\0'), to the buffer pointed to by dest.  The strings
 * may not overlap, and the destination string dest must be large enough to
 * receive the copy. */
char *strcpy(char *dest, const char *src);

/* The strcmp() function compares the two strings s1 and s2.  It returns an
 * integer less than, equal to, or greater than zero if s1 is found,
 * respectively, to be less than, to match, or be greater than s2. */
int strcmp(const char *s1, const char *s2);

uint8_t *memcpy(uint8_t *destination, uint32_t length, const uint8_t *src);
uint8_t *memset(uint8_t *destination, uint32_t length, uint8_t value);

#endif /* _STRING_H */
