#include <nos/string.h>

/* Return the length of a string. If passed a NULL pointer, returns -1. */
size_t strlen(const char *string)
{
  size_t length;

  length = 0;

  if (!string) {
    return -1;
  }

  while (*(string+length) != '\0') {
    length++;
  }

  return length;
}

char *strcpy(char *dest, const char *src)
{
  do {
    *dest++ = *src++;
  } while (*src != '\0');

  return dest;
}

int strcmp(const char *s1, const char *s2)
{
  int i = 0;

  while (s1[i] != '\0' && s2[i] != '\0') {
    if (s1[i] != s2[i]) {
      return ((s1[i] < s2[2]) ? -1 : 1);
    }
    i++;
  }

  return 0;
}

void *memcpy(void *destination, uint32_t length, const uint8_t *source)
{
  const uint8_t *source_p;
  uint8_t *destination_p;

  source_p = source;
  destination_p = destination;

  for ( ; length != 0; length--) {
    *destination_p++ = *source_p++;
  }

  return destination;
}

/* Write 'length' copies of 'value' to 'destination'. */
void *memset(void *destination, uint32_t length, uint8_t value)
{
  uint8_t *destination_p;

  destination_p = destination;

  for ( ; length != 0; length--) {
    *destination_p++ = value;
  }

  return destination;
}
