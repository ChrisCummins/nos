#include <kheap.h>

#include <assert.h>
#include <paging.h>
#include <util.h>
#include <kstream.h>

/* Heap functions. */
#define is_supervisor_only(heap) ((heap->supervisor_only) ? 1 : 0)
#define is_read_only(heap) ((heap->read_only)       ? 1 : 0)
#define sizeof_heap(heap) (heap->end_address - heap->start_address)

/* Block utility functions and constants. */
#define is_hole(h) ((h->is_hole) ? 1 : 0)
#define HEADER_SIZE (sizeof(struct header_s))
#define FOOTER_SIZE (sizeof(struct footer_s))
#define BLOCK_OVERHEAD (HEADER_SIZE + FOOTER_SIZE)

/* An invalid memory access in a heap algorithm can go totally undetected, so in
 * order to provde some level of validation that a piece of memory is in fact a
 * header or footer, we will define a symbolic constant that will be embedded
 * inside of each header and footer. Note that their value is purely
 * arbritary. */
#define HEADER_ID 0xCEC004B3
#define FOOTER_ID 0xCEC6F0FA

/* Tests that can be used to verify that a piece of memory is in fact a header
 * or footer, reliant on the _ID value. Obviously this is not a guaranteed
 * check, as it is possible that the specific piece of memory used to store the
 * ID could coincidentally (or othersie) contain the correct value. */
#define is_header(h) ((((struct header_s*)h)->id == HEADER_ID) ? 1 : 0)
#define is_footer(f) ((((struct footer_s*)f)->id == FOOTER_ID) ? 1 : 0)

/* Aligns an address to the next page. It does this by first masking off the
 * unaligned bits, and then incrmenting this address by the size of a page.
 *
 * For example:
 *    page size = 0x1000 <= our page size is 12 bits, so the trailing twelve
 *                          bits of any page aligned address will always be
 *                          zero.
 *    address = 0x08A00542 <= here we can see that this address is not page
 *                            aligned.
 *    addresss &= 0xFFFFF000 <= we start by masking off the last 12 digits of
 *                              the address, meaning that it is now page
 *                              aligned.
 *    address += 0x1000 <= finally, we must increment our address by the size
 *                         of a page, so that we are not pointing to previously
 *                         assigned memory.
 */
#define align_to_page(a) (a) &= ALIGNMENT_MASK; (a) += PAGE_SIZE

/* Used in the internal _kmalloc() function to determine whether to align
 * allocated memory to a page or not. */
enum align_page_e {
	NO_ALIGN,
	ALIGN_PAGE
};

/* This is defined in ./paging.c. */
extern struct page_directory_s *kernel_directory;
/* This is defined in ./link.ld. */
extern uint32_t end;

/* These are referenced in ./paging.c. */
uint32_t placement_address = (uint32_t)&end;
struct heap_s *kernel_heap;

static uint32_t _heap_kmalloc(uint32_t size, enum align_page_e align,
                              uint32_t *physical_address)
{
	type_t address = heap_alloc(kernel_heap, size, align);

	if (physical_address != 0) {
		struct page_s *page = get_page((uint32_t)address, 0,
					       kernel_directory);

		*physical_address = (page->frame * PAGE_SIZE)
			+ ((uint32_t)address & 0xFFF);
	}

	return (uint32_t)address;
}

static uint32_t _placement_kmalloc(uint32_t size, enum align_page_e align,
                                   uint32_t *physical_address)
{
	uint32_t destination_address;

	/* Align the placement address if necessary. */
	if ((align == ALIGN_PAGE) && !is_page_aligned(placement_address)) {
		align_to_page(placement_address);
	}

	if (physical_address) {
		*physical_address = placement_address;
	}

	destination_address = placement_address;
	placement_address += size;

	return destination_address;
}

/* Allocate a chunk of memory of size 'size'. If 'align' is set, the chunk must
 * be page-aligned. If 'physical_address' is nonzero, the allocated chunk will
 * be stored at 'physical_address'. */
static uint32_t _kmalloc(uint32_t size, enum align_page_e align,
                         uint32_t *physical_address)
{
	if (kernel_heap) {
		return _heap_kmalloc(size, align, physical_address);
	} else {
		return _placement_kmalloc(size, align, physical_address);
	}
}

/* Claim extra space for the heap. */
static void _heap_expand(struct heap_s *heap, uint32_t new_size)
{
	uint32_t old_size = sizeof_heap(heap);
	uint32_t i = old_size;

	assert(new_size > old_size);

	/* Align to the nearest following page boundary. */
	if (!is_page_aligned(new_size)) {
		align_to_page(new_size);
	}

	/* Return nothing if we attempt to expand heap larger than max
	 * address. */
	if (heap->start_address + new_size <= heap->max_address) {
		k_warn("Attempting to expand heap [%p] beyond maximum size (%h)",
		       heap, new_size);
		return;
	}

	/* Allocate extra frames as necessary. */
	while(i < new_size) {
		alloc_frame(get_page(heap->start_address+i, 1, kernel_directory),
			    is_supervisor_only(heap), !is_read_only(heap));

		i += PAGE_SIZE; /* Increment by page size. */
	}

	/* Set our new heap end address. */
	heap->end_address = heap->start_address + new_size;
}

/* Contract the heap. Returns the new size. */
static uint32_t _heap_contract(struct heap_s *heap, uint32_t new_size)
{
	uint32_t old_size = sizeof_heap(heap);
	uint32_t i = old_size - PAGE_SIZE;

	assert(new_size < old_size);

	/* Align to the nearest following page boundary. */
	if (!is_page_aligned(new_size)) {
		align_to_page(new_size);
	}

	/* Prevent over-contracting. */
	new_size = max(new_size, HEAP_MIN_SIZE);

	/* Free frames as necessary. */
	while (new_size < i) {
		free_frame(get_page(heap->start_address + i, 0,
				    kernel_directory));
		i -= PAGE_SIZE;
	}

	/* Set our new heap end address. */
	heap->end_address = heap->start_address + new_size;

	return new_size;
}

/* Find the smallest hole that will fit and return its index. If none is found,
 * return -1. */
static sint32_t _heap_find_first_fit(struct heap_s *heap, uint32_t size,
                                     uint8_t page_align)
{
	uint32_t i = 0;

	while (i < heap->index.size) {
		struct header_s *header;

		header = (struct header_s *)ordered_array_lookup_index(&heap->index, i);

		if (page_align) {
			uint32_t location = (uint32_t)header;
			sint32_t offset = 0;
			sint32_t hole_size;

			/* Page align the starting point of the header. Note
			 * that when a user requests that memory be
			 * page-aligned, that request applies only to memory
			 * that is user accessible. That means that the header
			 * address will actually not be page-aligned. The
			 * address that we want to fall on a boundary is the
			 * block location offset by the size of the header. */
			if (!is_page_aligned(location + HEADER_SIZE)) {
				offset = PAGE_SIZE
					- ((location
					    + HEADER_SIZE) % PAGE_SIZE);
			}

			hole_size = (sint32_t)header->size - offset;

			if (hole_size >= (sint32_t)size) {
				/* Stop if hole is large enough. */
				break;
			}
		} else if (header->size >= size) {
			/* Stop if hole is large enough. */
			break;
		}

		i++;
	}

	if (i == heap->index.size) {
		/* We reached the end and didn't find a large enough hole. */
		return -1;
	} else {
		return i;
	}
}

/* Return 1 if struct header_s a is larger, else return 0. Used to order a set
 * of headers by size, as opposed to the pointer address. */
sint8_t header_predicate(void *a, void *b)
{
	struct header_s *header_a = (struct header_s *)a;
	struct header_s *header_b = (struct header_s *)b;

	return (header_a->size > header_b->size) ? 1 : 0;
}

/* Create a heap. */
struct heap_s *heap_create(uint32_t start_address, uint32_t end_address,
                           uint32_t max_address, uint8_t supervisor_only,
                           uint8_t read_only)
{
	struct heap_s   *heap;
	struct header_s *hole;

	/* If we're not page aligned, then what has it all been for?? */
	assert((start_address % PAGE_SIZE) == 0);
	assert((end_address   % PAGE_SIZE) == 0);

	heap = kcreate(struct heap_s, 1);

	/* Initialise the index as an ordered array. */
	heap->index = ordered_array_place((void*)start_address, HEAP_INDEX_SIZE,
					  &header_predicate);

	/* Shift the start address forward to where we can start putting
	 * data. */
	start_address += sizeof(type_t) * HEAP_INDEX_SIZE;

	/* Ensure the start address is page-aligned. */
	if (!is_page_aligned(start_address)) {
		align_to_page(start_address);
	}

	heap->start_address = start_address;
	heap->end_address = end_address;
	heap->max_address = max_address;
	heap->supervisor_only = supervisor_only;
	heap->read_only = read_only;

	/* Create an initial block, which is a hole the size of the heap. */
	hole = (struct header_s *)start_address;
	hole->size = end_address - start_address;
	hole->id = HEADER_ID;
	hole->is_hole = 1;
	ordered_array_insert(&heap->index, (void*)hole);

	return heap;
}

/* Allocate a block of size 'size' from 'heap'. Align block to page if
 * 'page_align' is nonzero. */
void *heap_alloc(struct heap_s *heap, uint32_t size, uint8_t page_align)
{
	uint32_t total_size;
	sint32_t i;

	struct header_s *original_hole_header;
	uint32_t original_hole_position;
	uint32_t original_hole_size;

	struct header_s *block_header;
	struct footer_s *block_footer;

	/* We must account for the size of the header and footer. */
	total_size = (size + BLOCK_OVERHEAD);
	/* Find the smallest hole that fits. */
	i = _heap_find_first_fit(heap, total_size, page_align);

	if (i == -1) {
		struct header_s *header;
		struct footer_s *footer;
		uint32_t old_end_address = heap->end_address;
		uint32_t old_length = sizeof_heap(heap);
		uint32_t new_length;
		uint32_t index = -1;
		uint32_t value = 0;

		/* We must allocate some more space. */
		_heap_expand(heap, old_length + total_size);
		new_length = heap->end_address - heap->start_address;

		/* Find the endmost header. N.B. this is the endmost in location, not in
		 * size. We do this by iterating over all of the headers. */
		i = 0;
		while ((uint32_t)i < heap->index.size) {
			uint32_t temp;

			temp = (uint32_t)ordered_array_lookup_index(&heap->index, i);

			if (temp > value) {
				value = temp;
				index = i;
			}

			i++;
		}

		if (index == (uint32_t)-1) {
			/* If we didn't find any headers, we must add one. */
			header = (struct header_s *)old_end_address;
			header->id = HEADER_ID;
			header->size = new_length - old_length;
			header->is_hole = 1;

			footer = (struct footer_s *)(old_end_address
						     + header->size
						     - FOOTER_SIZE);
			footer->id = FOOTER_ID;
			footer->header = header;

			ordered_array_insert((void*)header, &heap->index);
		} else {
			/* The last header needs adjusting. */
			header = ordered_array_lookup_index(&heap->index, index);
			header->size += new_length - old_length;

			/* Re-write the footer. */
			footer = (struct footer_s*)((uint32_t)header
						    + header->size
						    - FOOTER_SIZE);
			footer->id = FOOTER_ID;
			footer->header = header;
		}

		/* We now have enough space, so can recurse. */
		heap_alloc(heap, size, page_align);
	}

	/* We must now decide whether to split the hole we found into two
	 * parts. Is the original hole size minus requested hole size less than
	 * the overhead for adding a new hole? */
	original_hole_header = (struct header_s*)ordered_array_lookup_index(&heap->index, i);
	original_hole_position = (uint32_t)original_hole_header;
	original_hole_size = original_hole_header->size;

	if ((original_hole_size - total_size) < BLOCK_OVERHEAD) {
		/* Increae the requested size to the size of the hole we
		 * found. */
		size += original_hole_size - total_size;
		total_size = original_hole_size;
	}

	/* If we need to page-align the data, do it now and make a new hole in
	 * front of our block. */
	if (page_align && (!is_page_aligned(original_hole_position))) {
		uint32_t new_location;
		struct header_s *hole_header;
		struct footer_s *hole_footer;

		new_location = (original_hole_position + PAGE_SIZE
				- HEADER_SIZE
				- (original_hole_position & 0xFFF));

		hole_header = (struct header_s*)original_hole_position;
		hole_header->size = (PAGE_SIZE
				     - (original_hole_position & 0xFFF)
				     - HEADER_SIZE);
		hole_header->id = HEADER_ID;
		hole_header->is_hole = 1;

		hole_footer = (struct footer_s*)((uint32_t)new_location - FOOTER_SIZE);
		hole_footer->id = FOOTER_ID;
		hole_footer->header = hole_header;
		original_hole_position = new_location;
		original_hole_size = original_hole_size - hole_header->size;
	} else {
		/* We don't need this hole anymore, so remove it from the
		 * index. */
		ordered_array_remove_index(&heap->index, i);
	}

	/* Overwrite the original header. */
	block_header = (struct header_s*)original_hole_position;
	block_header->id = HEADER_ID;
	block_header->is_hole = 0;
	block_header->size = total_size;

	/* Overwrite the original footer. */
	block_footer = (struct footer_s*)(original_hole_position
					  + HEADER_SIZE + size);
	block_footer->id = FOOTER_ID;
	block_footer->header = block_header;

	/* We may need to write a new hole after the allocated block. We do this only
	 * if the new hole would have a positive size (size > 0). */
	if ((original_hole_size - total_size) > 0) {
		struct header_s *hole_header;
		struct footer_s *hole_footer;

		hole_header = (struct header_s*)(original_hole_position + size
						 + BLOCK_OVERHEAD);
		hole_header->id = HEADER_ID;
		hole_header->is_hole = 1;
		hole_header->size = original_hole_size - total_size;

		hole_footer = (struct footer_s*)((uint32_t)hole_header
						 - total_size
						 + original_hole_size
						 - FOOTER_SIZE);
		if ((uint32_t)hole_footer < heap->end_address) {
			hole_footer->id = FOOTER_ID;
			hole_footer->header = hole_header;
		}

		/* Put the new hole in the index. */
		ordered_array_insert(&heap->index, (void*)hole_header);
	}

	return (void*)((uint32_t)block_header + HEADER_SIZE);
}

void heap_free(struct heap_s *heap, void *block)
{
	struct header_s *header;
	struct footer_s *footer;
	struct header_s *test_header;
	struct footer_s *test_footer;
	char do_add = 1;

	/* Exit gracefully for a null pointer. */
	if (!block) {
		return;
	}

	/* Retrieve the header and footer associated with this pointer. */
	header = (struct header_s*)((uint32_t)block - HEADER_SIZE);
	footer = (struct footer_s*)((uint32_t)header
				    + header->size
				    - FOOTER_SIZE);

	/* Verify that we have a true block. */
	assert(is_header(header));
	assert(is_footer(footer));

	/* Create a hole. */
	header->is_hole = 1;

	/* Now come decision logic as to whether to index this as a free
	 * hole. */
	test_footer = (struct footer_s*)((uint32_t)header - FOOTER_SIZE);

	/* If the memory immediately to the left of this block is a footer then
	 * we can merge left. */
	if (is_footer(test_footer) && is_hole(test_footer->header)) {
		uint32_t cache_size = header->size;

		/* Cache our current size, and proceed to re-write the current
		 * header with the new one, re-write the footer to point to the
		 * new header, update the header size, and set the flag so that
		 * we don't add to index again. */
		header = test_footer->header;
		footer->header = header;
		header->size += cache_size;
		do_add = 0;
	}

	/* If the memory immediately to the right of this block is a footer then
	 * we can merge right. */
	test_header = (struct header_s*)((uint32_t)footer + FOOTER_SIZE);
	if (test_header->id == HEADER_ID
	    && test_header->is_hole == 1) {
		uint32_t i = 0;

		/* Increase the size the current header, and re-write the footer
		 * of the test block to point to the current header. */
		header->size += test_header->size;
		test_footer = (struct footer_s*)((uint32_t)test_header
						 + test_header->size
						 - FOOTER_SIZE);
		footer = test_footer;

		/* Find and remove this header from the index. */
		while ((i < heap->index.size)
		       && (ordered_array_lookup_index(&heap->index, i)
			   != (void*)test_header)) {
			i++;
		}

		/* Ensure that we found the correct header. */
		assert(i < heap->index.size);

		/* Remove header from index. */
		ordered_array_remove_index(&heap->index, i);
	}

	/* If the location of the footer is the end address, we can contract. */
	if ((uint32_t)footer + FOOTER_SIZE == heap->end_address) {
		uint32_t old_length = (heap->end_address - heap->start_address);
		uint32_t new_length = _heap_contract(heap, (uint32_t)header
						     - heap->start_address);

		/* Check how big the block will be after resizing. */
		if (header->size - (old_length - new_length) > 0) {
			/* The block will still exist, so resize. */
			header->size -= (old_length - new_length);

			footer = (struct footer_s*)((uint32_t)header
						    + header->size
						    - FOOTER_SIZE);
			footer->id = FOOTER_ID;
			footer->header = header;
		} else {
			uint32_t i = 0;

			/* The block will no longer exist, so RIP block, and
			 * remove from index. */
			while ((i < heap->index.size)
			       && (ordered_array_lookup_index(&heap->index, i)
				   != (void*)test_header)) {
				i++;
			}

			/* If we didn't find the block, there is nothing to
			 * remove. */
			if (i < heap->index.size) {
				ordered_array_remove_index(&heap->index, i);
			}
		}
	}

	/* If required, add this block to the index. */
	if (do_add == 1) {
		ordered_array_insert(&heap->index, (void*)header);
	}
}

uint32_t kmalloc(uint32_t size)
{
	return _kmalloc(size, NO_ALIGN, 0);
}

uint32_t kmalloc_a(uint32_t size)
{
	return _kmalloc(size, ALIGN_PAGE, 0);
}

uint32_t kmalloc_p(uint32_t size, uint32_t *physical_address)
{
	return _kmalloc(size, NO_ALIGN, physical_address);
}

uint32_t kmalloc_ap(uint32_t size, uint32_t *physical_address)
{
	return _kmalloc(size, ALIGN_PAGE, physical_address);
}

void kfree(void *block)
{
	heap_free(kernel_heap, block);
}
