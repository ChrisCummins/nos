#include <nos/gdt.h>
#include <nos/idt.h>
#include <nos/tty.h>
#include <nos/timer.h>
#include <nos/paging.h>
#include <nos/string.h>
#include <nos/kstream.h>

struct multiboot_s;

/* We don't want GCC complaining if we don't use the registers parameter. */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

int kmain(struct multiboot_s *mboot_ptr)
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

#pragma GCC diagnostic pop /* ignored "-Wunused-parameter" */
