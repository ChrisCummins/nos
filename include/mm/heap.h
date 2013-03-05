#ifndef _HEAP_H
#define _HEAP_H

#include <kernel/types.h>
#include <lib/ordered-array.h>

/* Limits and characteristics of the heap. These are arbritary and
 * customiseable values. */
#define KHEAP_START         0xC0000000
#define KHEAP_MAX           0xCFFFF000
#define KHEAP_INITIAL_SIZE  0x100000
#define HEAP_INDEX_SIZE     0x20000
#define HEAP_MIN_SIZE       0x70000

struct header {
	uint32_t id;     /* Symbolic constant, used for identification.   */
	uint8_t is_hole; /* 1=hole, 0=block.                              */
	uint32_t size;   /* Size of the block, including the end footer.  */
};

struct footer {
	uint32_t id; /* Symbolic constant, used for identification. */
	struct header *header; /* Pointer to the block header.    */
};

struct heap {
	struct ordered_array index;
	uint32_t start_address;   /* Start address of our allocated space.         */
	uint32_t end_address;     /* End address of our allocated space.           */
	uint32_t max_address;     /* Maximum address that the heap can expand to.  */
	uint8_t supervisor_only;  /* Should extra pages be mapped supervisor-only? */
	uint8_t read_only;        /* Should extra pages be mapped read-only?       */
};

struct heap *heap_create(uint32_t start_address, uint32_t end_address,
			 uint32_t max_address, uint8_t supervisor_only,
			 uint8_t read_only);

/* Allocate and free blocks. */
void *alloc(struct heap *heap, uint32_t size, uint8_t page_align);
void free(struct heap *heap, void *block);

/* Heap allocation and deallocation. */
uint32_t kmalloc(uint32_t size);
uint32_t kmalloc_a(uint32_t size);
uint32_t kmalloc_p(uint32_t size, uint32_t *physical_address);
uint32_t kmalloc_ap(uint32_t size, uint32_t *physical_address);
void kfree(void *block);

#define kcreate(type, count)						\
	(type *)kmalloc(sizeof(type) * (count))

#define kcreate_a(type, count)						\
	(type *)kmalloc_a(sizeof(type) * (count))

#define kcreate_p(type, count, physical_address)			\
	(type *)kmalloc_p(sizeof(type) * (count), (physical_address))

#define kcreate_ap(type, count, physical_address)			\
	(type *)kmalloc_ap(sizeof(type) * (count), (physical_address))

#endif /* _HEAP_H */
