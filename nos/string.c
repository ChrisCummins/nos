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
