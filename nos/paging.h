#ifndef _PAGING_H
#define _PAGING_H

#include <nos/types.h>
#include <nos/isr.h>

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
  struct page_s pages[1024];
};

struct page_directory_s {
  struct page_table_s *virtual_tables[1024];
  uint32_t             physical_address[1024];
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

#endif /* _PAGING_H */
