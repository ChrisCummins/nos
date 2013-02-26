#ifndef _PAGING_H
#define _PAGING_H

#include <nos/isr.h>
#include <nos/types.h>

/* The size of a page. */
#define PAGE_SIZE 0x1000

/* This value can be used to determine if an address falls on a page boundary or
 * not, by performing a logical AND of the given address and this mask. If the
 * address falls on a page boundary, then the masking operation will zero all
 * bits. */
#define ALIGNMENT_MASK 0xFFFFF000

/* Determines whether an address is page aligned or not. To do so, it ANDs the
 * value with an alignment ask. If the address is page aligned, then the
 * alignment mask will mask off all nonzero digits. If the address does not fall
 * on a page boundary, then the index into the page will remain. */
#define is_page_aligned(a)  ((((a) & ALIGNMENT_MASK) == 0) ? 1 : 0)

/* Here we set some limits and sizes to our memory. */
#define PAGES_IN_TABLE      1024
#define TABLES_IN_DIRECTORY 1024
#define MEMORY_END_PAGE     0x01000000

typedef uint32_t frame_t;

struct page_s {
  uint32_t present  :  1; /* Page present in memory. */
  uint32_t rw       :  1; /* Read-only if clear, readwrite if set. */
  uint32_t user     :  1; /* Supervisor level only if clear. */
  uint32_t accessed :  1; /* Has the page been accessed since last refresh? */
  uint32_t dirty    :  1; /* Has the page been written to since last refresh? */
  uint32_t unused   :  7; /* Amalgamation of unused and reserved bits. */
  uint32_t frame    : 20; /* Frame address (shifted right 12 bits). */
};

struct page_table_s {
  struct page_s pages[PAGES_IN_TABLE];
};

struct page_directory_s {
  struct page_table_s *virtual_tables[TABLES_IN_DIRECTORY];
  uint32_t             physical_address[TABLES_IN_DIRECTORY];
  uint32_t             directory_address;
};

void init_paging(void);

/* Load the specified page directory into CR3 register. */
void switch_page_directory(struct page_directory_s *new);

/* Determines whether to create a page-table if necessary. */
enum create_page_e {
  NO_CREATE   = 0,
  CREATE_PAGE = 1
};

/* Retrieve page from page directory. */
struct page_s *get_page(uint32_t address, enum create_page_e make,
                        struct page_directory_s *page_directory);

/* Handler for page faults. */
void page_fault(struct registers_s registers);

void alloc_frame(struct page_s *page, int is_kernel, int is_writeable);
void free_frame (struct page_s *page);

#endif /* _PAGING_H */
