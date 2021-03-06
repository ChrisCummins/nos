#ifndef _PAGING_H
#define _PAGING_H

#include <kernel/isr.h>
#include <kernel/types.h>
#include <lib/stdio.h>

/* Define this for paging debugging. */
#define PAGING_DEBUG 1

/* The size of a page. */
#define PAGE_SIZE 0x1000

/* This value can be used to determine if an address falls on a page boundary or
 * not, by performing a logical AND of the given address and this mask. If the
 * address falls on a page boundary, then the masking operation will zero all
 * bits. */
#define ALIGNMENT_MASK 0xFFFFF000

/* A logical OR of an address with this mask will zero all but the trailing 12
 * bits, leaving the offset into the page. */
#define PAGE_OFFSET_MASK 0xFFF

/* Determines whether an address is page aligned or not. To do so, it ANDs the
 * value with an alignment ask. If the address is page aligned, then the
 * alignment mask will mask off all nonzero digits. If the address does not fall
 * on a page boundary, then the index into the page will remain. */
#define is_page_aligned(a) ((((a) & ALIGNMENT_MASK) == 0) ? 1 : 0)

/* Here we set some limits and sizes to our memory. */
#define PAGES_IN_TABLE 1024
#define TABLES_IN_DIRECTORY 1024
#define MEMORY_END_PAGE 0x01000000

/* If we reach here, we're out of memory! */
#define MEMORY_END_FRAME 0xFFFFFFFF

#ifdef PAGING_DEBUG
# define paging_debug(...) {				\
		kdebug("%s:%d, %s() ",			\
		       __FILE__, __LINE__, __func__);	\
		kdebug(__VA_ARGS__);			\
	}
#else
# define paging_debug(f, ...) /**/
#endif

struct page {
	uint32_t present  :  1; /* Page present in memory. */
	uint32_t rw       :  1; /* Read-only if clear, readwrite if set. */
	uint32_t user     :  1; /* Supervisor level only if clear. */
	uint32_t accessed :  1; /* Has the page been accessed since last refresh? */
	uint32_t dirty    :  1; /* Has the page been written to since last refresh? */
	uint32_t unused   :  7; /* Amalgamation of unused and reserved bits. */
	uint32_t frame    : 20; /* Frame address (shifted right 12 bits). */
};

struct page_table {
	struct page pages[PAGES_IN_TABLE];
};

struct page_directory {
	struct page_table *virtual_tables[TABLES_IN_DIRECTORY];
	uint32_t physical_address[TABLES_IN_DIRECTORY];
	uint32_t directory_address;
};

void init_paging(void);

/* Load the specified page directory into CR3 register. */
void switch_page_directory(struct page_directory *new);

/* Determines whether to create a page-table if necessary. */
enum create_page_e {
	NO_CREATE = 0,
	CREATE_PAGE = 1
};

/* Retrieve page from page directory. */
struct page *get_page(uint32_t address, enum create_page_e make,
		      struct page_directory *page_directory);

/* Handler for page faults. */
void page_fault(struct registers registers);

void alloc_frame(struct page *page, int is_kernel, int is_writeable);
void free_frame(struct page *page);

struct page_directory *clone_directory(struct page_directory *src);


#endif /* _PAGING_H */
