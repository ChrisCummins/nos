#include <nos/port.h>

void out_byte(port_t port, uint8_t value)
{
  __asm volatile("outb %1, %0" : : "dN" (port), "a" (value));
}

uint8_t in_byte(port_t port)
{
  uint8_t return_value;

  __asm volatile("inb %1, %0" : "=a" (return_value) : "dN" (port));
  return return_value;
}

uint16_t in_word(port_t port)
{
  uint16_t return_value;

  __asm volatile("inw %1, %0" : "=a" (return_value) : "dN" (port));
  return return_value;
}
