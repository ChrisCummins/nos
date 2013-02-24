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

/* Copy 'length' bytes from 'source' to 'destiantion' */
void memory_copy(void *destination, uint32_t length, const uint8_t *source)
{
  const uint8_t *source_p;
  uint8_t *destination_p;

  source_p = source;
  destination_p = destination;

  for ( ; length != 0; length--) {
    *destination_p++ = *source_p++;
  }
}

/* Write 'length' copies of 'value' to 'destination'. */
void memory_set(void *destination, uint32_t length, uint8_t value)
{
  uint8_t *destination_p;

  destination_p = destination;

  for ( ; length != 0; length--) {
    *destination_p++ = value;
  }
}
