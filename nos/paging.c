#include <paging.h>

#include <kheap.h>
#include <kstream.h>
#include <panic.h>
#include <string.h>
#include <tty.h>
#include <util.h>

/* Macros used in the bitset algorithms. */
#define INDEX_FROM_BIT(a)  (a / (8 * 4))
#define OFFSET_FROM_BIT(a) (a % (8 * 4))

/* Defined in ./process.s. */
extern void copy_page_physical(uint32_t src, uint32_t dest);

struct page_directory_s *kernel_directory  = 0;
struct page_directory_s *current_directory = 0;

/* A bitset of frames, used or free. */
static uint32_t *frames;
static uint32_t frames_count;

/* Defined in ./kheap.c. */
extern uint32_t       placement_address;
extern struct heap_s *kernel_heap;

/* Set a bit in a frame's bitset. */
static void _set_frame(uint32_t frame_address)
{
	uint32_t frame;
	uint32_t i;
	uint32_t offset;

	frame = frame_address / PAGE_SIZE;
	i = INDEX_FROM_BIT(frame);
	offset = OFFSET_FROM_BIT(frame);

	frames[i] |= (0x1 << offset);
}

/* Clear a bit in a frame's bitset. */
static void _clear_frame(uint32_t frame_address)
{
	uint32_t frame;
	uint32_t i;
	uint32_t offset;

	frame = frame_address / PAGE_SIZE;
	i = INDEX_FROM_BIT(frame);
	offset = OFFSET_FROM_BIT(frame);

	frames[i] &= ~(0x1 << offset);
}

/* Test if a bit is set. */
/* static uint32_t _test_frame(uint32_t frame_address) */
/* { */
/*   uint32_t  frame; */
/*   uint32_t index; */
/*   uint32_t offset; */

/*   frame  = frame_address / PAGE_SIZE; */
/*   index  = INDEX_FROM_BIT(frame); */
/*   offset = OFFSET_FROM_BIT(frame); */

/*   return (frames[index] & (0x1 << offset)); */
/* } */

/* Find the first free frame. */
static uint32_t _first_frame(void)
{
	uint32_t i;
	uint32_t j;

	for (i = 0; i < INDEX_FROM_BIT(frames_count); i++) {
		if (frames[i] != 0xFFFFFFFF) {
			/* At least one bit is free in this frame. */
			for (j = 0; j < (sizeof(uint32_t) * 8); j++) {
				uint32_t test;

				test = 0x1 << j;
				if (!(frames[i] & test)) {
					return ((i * sizeof(uint32_t) * 8) + j);
				}
			}
		}
	}

	return 0;
}

static struct page_table_s *clone_table(struct page_table_s *src,
                                        uint32_t *physical_address)
{
	struct page_table_s *table;
	int i;

	/* Make and zero a page aligned table. */
	table = kcreate_ap(struct page_table_s, 1, physical_address);
	memset((uint8_t *)table, 0x0, sizeof(struct page_directory_s));

	/* Iterate over the table entries. */
	for (i = 0; i < PAGES_IN_TABLE; i++) {
		/* If the source entry has a frame associated with it, then copy it. */
		if (src->pages[i].frame) {
			/* Get a new frame. */
			alloc_frame(&table->pages[i], 0, 0);

			/* Clone the flags from src to dst. */
			src->pages[i].present = table->pages[i].present;
			src->pages[i].rw = table->pages[i].rw;
			src->pages[i].user = table->pages[i].user;
			src->pages[i].accessed = table->pages[i].accessed;
			src->pages[i].dirty = table->pages[i].dirty;

			/* Physically copy the data accross. */
			copy_page_physical(src->pages[i].frame * PAGE_SIZE, table->pages[i].frame * PAGE_SIZE);
		}
	}

	return table;
}

void init_paging()
{
	uint32_t i;

	paging_debug("initialising paging");

	/* Get the number of frames. */
	frames_count = MEMORY_END_PAGE / PAGE_SIZE;
	frames       = (uint32_t *)kmalloc(INDEX_FROM_BIT(frames_count));
	memset((void *)frames, 0x0, INDEX_FROM_BIT(frames_count));

	/* Make a page directory. */
	kernel_directory  = kcreate_a(struct page_directory_s, 1);
	memset((uint8_t*)kernel_directory, 0x0, sizeof(struct page_directory_s));
	kernel_directory->directory_address = (uint32_t)kernel_directory->physical_address;

	/* Map some pages in the kernel heap area, using get_page() not
	 * alloc_frame(). This causes struct page_table_s' to be created where
	 * necessary. We can't allocate frames yet because they need to be identity
	 * mapped first below, and yet we can't increase placement_address between
	 * identifying mapping and enabling the heap. */
	i = 0;
	for (i = KHEAP_START; i < KHEAP_START + KHEAP_INITIAL_SIZE; i += PAGE_SIZE) {
		get_page(i, 1, kernel_directory);
	}

	/* We need to identity map (physical_address = virtual address) from 0x0 to
	 * the end of the used memory, so that we can access this transparently, as if
	 * paging weren't enabled. An extra page is allocated so that the kernel heap
	 * can be initialised properly. */
	i = 0;
	while (i < placement_address + PAGE_SIZE) {
		/* Kernel code is read only from user-space. */
		alloc_frame(get_page(i, 1, kernel_directory), 0, 0);
		i += PAGE_SIZE;
	}

	/* Now allocate the pages mapped earlier. */
	for (i = KHEAP_START; i < KHEAP_START + KHEAP_INITIAL_SIZE; i += PAGE_SIZE) {
		alloc_frame(get_page(i, 1, kernel_directory), 0, 0);
	}

	/* Register our page fault handler. */
	register_interrupt_handler(14, page_fault);

	/* Enable paging. */
	switch_page_directory(kernel_directory);

	/* Initialise the kernel heap. */
	kernel_heap = heap_create(KHEAP_START, KHEAP_START + KHEAP_INITIAL_SIZE,
				  KHEAP_MAX, 0, 0);

	current_directory = clone_directory(kernel_directory);
	switch_page_directory(current_directory);
}

/* Allocate a frame. */
void alloc_frame(struct page_s *page, int is_kernel, int is_writeable)
{
	if (page->frame == 0) {
		uint32_t index;

		index = _first_frame();
		if (index == (uint32_t)-1) {
			k_critical("Unable to allocate a frame for page %p", page);
			panic("Out of Memory");
		}

		_set_frame(index * PAGE_SIZE);
		page->present = 1;
		page->rw      = (is_writeable) ? 1 : 0;
		page->user    = (is_kernel)    ? 1 : 0;
		page->frame   = index;
	}
}

/* Deallocate a frame. */
void free_frame(struct page_s *page)
{
	uint32_t frame;

	if ((frame = page->frame)) {
		_clear_frame(frame);
		page->frame = 0x0;
	}
}

void switch_page_directory(struct page_directory_s *dir)
{
	uint32_t cr0;

	paging_debug("Switching page directory: %h -> %h", current_directory, dir);
	current_directory = dir;
	__asm volatile("mov %0, %%cr3":: "r"(dir->physical_address));
	__asm volatile("mov %%cr0, %0": "=r"(cr0));
	cr0 |= 0x80000000; /* Enable paging. */
	__asm volatile("mov %0, %%cr0":: "r"(cr0));
}

struct page_s *get_page(uint32_t address, enum create_page_e make,
                        struct page_directory_s *directory)
{
	uint32_t table_index;

	/* Turn address into an index. */
	address /= PAGE_SIZE;

	/* Find the page table containing this address. */
	table_index = address / TABLES_IN_DIRECTORY;

	if (directory->virtual_tables[table_index]) {
		/* If this table is already assigned. */
		return &directory->virtual_tables[table_index]->pages[address % PAGES_IN_TABLE];
	} else if (make == CREATE_PAGE) {
		uint32_t temp;

		/* Create a table. */
		directory->virtual_tables[table_index] = kcreate_ap(struct page_table_s, 1,
								    &temp);
		directory->physical_address[table_index] = temp | 0x7; /* Present, R/W,
									* User-space. */

		return &directory->virtual_tables[table_index]->pages[address % PAGES_IN_TABLE];
	} else {
		return 0;
	}
}

struct page_directory_s *clone_directory(struct page_directory_s *src)
{
	struct page_directory_s *dir;
	uint32_t dir_address;
	uint32_t offset;
	int i;

	/* Make and zero a page directory and obtain its physical address. */
	dir = kcreate_ap(struct page_directory_s, 1, &dir_address);
	k_debug("dir_address: %h", dir_address);
	memset((uint8_t*)dir, 0x0, sizeof(struct page_directory_s));

	/* Get the offset of physical_tables from the start of the struct
	 * page_directory_s. and add it the dir_address to get total offset. */
	offset = (uint32_t)dir->physical_address - (uint32_t)dir;
	dir->directory_address = dir_address + offset;

	/* Iterate through the page tables. If the page table is in the kernel
	 * directory (i.e., it is a kernel page), do not make a new copy. */
	for (i = 0; i < TABLES_IN_DIRECTORY; i++) {
		if (!src->virtual_tables[i]) {
			continue;
		}

		/* If the page table is in the kernel directory, take the existing
		 * pointer. */
		if (kernel_directory->virtual_tables[i] == src->virtual_tables[i]) {
			dir->virtual_tables[i] = src->virtual_tables[i];
			dir->physical_address[i] = src->physical_address[i];
		} else {
			uint32_t phys;
			/* Copy the table. */
			dir->virtual_tables[i] = clone_table(src->virtual_tables[i],
							     &phys);
			dir->physical_address[i] = phys | 0x07;
		}
	}

	paging_debug("Cloned page directory %h -> %h", src, dir);

	return dir;
}

void page_fault(struct registers_s registers)
{
	uint32_t faulting_address;
	int present;
	int rw;
	int us;
	int reserved;
	/* int  id; */

	/* A page fault has occurred. The fault address is stored in the CR2
	 * register. */
	__asm volatile("mov %%cr2, %0" : "=r" (faulting_address));

	/* Decode information from the error code. */
	present  = !(registers.error_code & 0x1);
	rw       = registers.error_code & 0x2;
	us       = registers.error_code & 0x4;
	reserved = registers.error_code & 0x8;
	/* id       = registers.error_code & 0x10; */

	k_critical("PAGE FAULT: %h ", faulting_address);

	if (present) {
		tty_write("PRESENT ");
	}

	if (rw) {
		tty_write("READ-ONLY ");
	}

	if (us) {
		tty_write("USER-MODE ");
	} else {
		tty_write("KERNEL-MODE ");
	}

	if (reserved) {
		tty_write("RESERVED ");
	}

	tty_write("\n");
	panic("Page fault");
}
