#include <nos/paging.h>
#include <nos/kheap.h>
#include <nos/memory.h>
#include <nos/kstream.h>
#include <nos/util.h>

struct page_directory_s *kernel_directory  = 0;
struct page_directory_s *current_directory = 0;

static frame_t  *frames;
static uint32_t  nframes;

/* Defined in ./kheap.c. */
extern uint32_t placement_address;

/* Macros used in the bitset algorithms. */
#define INDEX_FROM_BIT(a)  (a / (8 * 4))
#define OFFSET_FROM_BIT(a) (a % (8 * 4))

/* Set a bit in a frame's bitset. */
static void _set_frame(uint32_t frame_address)
{
  frame_t  frame;
  uint32_t index;
  uint32_t offset;

  frame  = frame_address / 0x1000;
  index  = INDEX_FROM_BIT(frame);
  offset = OFFSET_FROM_BIT(frame);

  frames[index] |= (0x1 << offset);
}

/* Clear a bit in a frame's bitset. */
static void _clear_frame(uint32_t frame_address)
{
  frame_t  frame;
  uint32_t index;
  uint32_t offset;

  frame  = frame_address / 0x1000;
  index  = INDEX_FROM_BIT(frame);
  offset = OFFSET_FROM_BIT(frame);

  frames[index] &= ~(0x1 << offset);
}

/* Test if a bit is set. */
static uint32_t _test_frame(uint32_t frame_address)
{
  frame_t  frame;
  uint32_t index;
  uint32_t offset;

  frame  = frame_address / 0x1000;
  index  = INDEX_FROM_BIT(frame);
  offset = OFFSET_FROM_BIT(frame);

  return (frames[index] & (0x1 << offset));
}

/* Find the first free frame. */
static frame_t first_frame(void)
{
  uint32_t i;
  uint32_t j;

  for (i = 0; i < INDEX_FROM_BIT(nframes); i++) {
    if (frames[i] != 0xFFFFFFFF) {
      /* At least one bit is free in this frame. */
      for (j = 0; j < (sizeof(frame_t) * 8); j++) {
        uint32_t test;

        test = 0x1 << j;
        if (!(frames[i] & test)) {
          return ((i * sizeof(frame_t) * 8) + j);
        }
      }
    }
  }

  return 0;
}

void init_paging()
{
  uint32_t i;
  uint32_t memory_end_page;

  k_message("Initialising Paging");

  /* Set a size for our physical memory. */
  memory_end_page = 0x1000000;

  /* Get the number of frames. */
  nframes = memory_end_page / 0x1000;
  frames  = (uint32_t *)kmalloc(INDEX_FROM_BIT(nframes));
  memory_set((uint8_t *)frames, INDEX_FROM_BIT(nframes), 0);

  /* Make a page directory. */
  kernel_directory  = kcreate_a(struct page_directory_s, 1);
  current_directory = kernel_directory;

  i = 0;
  while (i < placement_address) {
    /* Kernel code is read only from user-space. */
    alloc_frame(get_page(i, 1, kernel_directory), 0, 0);
    i += 0x1000;
  }

  /* Register our page fault handler. */
  register_interrupt_handler(14, page_fault);

  /* Enable paging. */
  switch_page_directory(kernel_directory);
}

/* Allocate a frame. */
void alloc_frame(struct page_s *page, int is_kernel, int is_writeable)
{
  if (page->frame == 0) {
    uint32_t index;

    index = first_frame();
    if (index == (uint32_t)-1) {
      /* TODO: No free frames. PANIC. */
    }

    _set_frame(index * 0x1000);
    page->present = 1;
    page->rw      = (is_writeable) ? 1 : 0;
    page->user    = (is_kernel)    ? 1 : 0;
    page->frame   = index;
  }
}

/* Deallocate a frame. */
void free_frame(struct page_s *page)
{
  frame_t frame;

  if ((frame = page->frame)) {
    _clear_frame(frame);
    page->frame = 0x0;
  }
}

void switch_page_directory(struct page_directory_s *directory)
{
  uint32_t cr0;

  current_directory = directory;
  __asm volatile("mov %0, %%cr3":: "r"(&directory->physical_address));
  __asm volatile("mov %%cr0, %0": "=r"(cr0));
  cr0 |= 0x80000000; /* Enable paging. */
  __asm volatile("mov %0, %%cr0":: "r"(cr0));
}

struct page_s *get_page(uint32_t address, enum create_page_e make,
                        struct page_directory_s *directory)
{
  uint32_t table_index;

  /* Turn address into an index. */
  address /= 0x1000;

  /* Find the page table containing this address. */
  table_index = address / 1024;

  if (directory->virtual_tables[table_index]) {
    /* If this table is already assigned. */
    return &directory->virtual_tables[table_index]->pages[address % 1024];
  } else if (make == CREATE_PAGE) {
    uint32_t temp;

    /* Create a table. */
    directory->virtual_tables[table_index] = kcreate_ap(struct page_table_s, 1,
                                                        &temp);
    directory->physical_address[table_index] = temp | 0x7; /* Present, R/W,
                                                            * User-space. */

    return &directory->virtual_tables[table_index]->pages[address % 1024];
  } else {
    return 0;
  }
}

void page_fault(struct registers_s registers)
{
  uint32_t faulting_address;
  int present;
  int rw;
  int us;
  int reserved;
  int id;

  /* A page fault has occurred. The fault address is stored in the CR2
   * register. */
  __asm volatile("mov %%cr2, %0" : "=r" (faulting_address));

  /* Decode information from the error code. */
  present  = !(registers.error_code & 0x1);
  rw       = registers.error_code & 0x2;
  us       = registers.error_code & 0x4;
  reserved = registers.error_code & 0x8;
  id       = registers.error_code & 0x10;

  k_critical("PAGE FAULT: %h", faulting_address);

  /* if (present) { */
  /*   tty_write("PRESENT "); */
  /* } */

  /* if (rw) { */
  /*   tty_write("READ-ONLY "); */
  /* } */

  /* if (us) { */
  /*   tty_write("USER-MODE "); */
  /* } else { */
  /*   tty_write("KERNEL-MODE "); */
  /* } */

  /* if (reserved) { */
  /*   tty_write("RESERVED "); */
  /* } */

  /* tty_write("]\n"); */

  PANIC("Page fault");
}
