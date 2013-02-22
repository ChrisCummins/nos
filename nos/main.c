#include <nos/gdt.h>
#include <nos/idt.h>
#include <nos/tty.h>
#include <nos/timer.h>
#include <nos/paging.h>
#include <nos/string.h>
#include <nos/kstream.h>

/* We don't want GCC complaining about malformed int main() arguments. */
#pragma GCC diagnostic ignored "-Wmain"

int kmain(struct multiboot *mboot_ptr)
{
  init_kstream();

  init_timer(50);
  init_idt();
  init_gdt();
  init_paging();

  /* Enable interrupts. */
  /* __asm volatile("sti"); */

  /* Cause a kernel panic. */
  /* uint32_t *ptr = (uint32_t*)0xA0000000; */
  /* uint32_t do_page_fault = *ptr; */

  return 0;
}
