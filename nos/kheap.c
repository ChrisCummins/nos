#include <nos/kheap.h>

/* Defined in ./link.ld */
extern uint32_t end;
uint32_t placement_address = (uint32_t)&end;

enum kmalloc_align_e {
  NO_PAGE_ALIGN = 0,
  ALIGN_PAGE    = 1
};

/* Allocate a chunk of memory of size 'size'. If align == 1, the chunk must be
 * page-aligned. If physical_address != 0, the physical location of the
 * allocated chunk will be stored into physical_address. */
static uint32_t _kmalloc(uint32_t size, enum kmalloc_align_e align,
                         uint32_t *physical_address)
{
  uint32_t address;

  /* This will eventually call malloc on the kernel heap. For now, we just
   * assign memory at placement_address and increment it by size. This is useful
   * before the heap is initialised. */
  if ((align == ALIGN_PAGE) && (placement_address & 0xFFFFF000)) {
    /* Align the placement address. */
    placement_address &= 0xFFFFF000;
    placement_address += 0x1000;
  }

  /* Return the physical address. */
  if (physical_address) {
    *physical_address = placement_address;
  }

  address = placement_address;
  placement_address += size;

  return address;
}

uint32_t kmalloc(uint32_t size)
{
  return _kmalloc(size, NO_PAGE_ALIGN, 0);
}

uint32_t kmalloc_a(uint32_t size)
{
  return _kmalloc(size, ALIGN_PAGE, 0);
}

uint32_t kmalloc_p(uint32_t size, uint32_t *physical_address)
{
  return _kmalloc(size, NO_PAGE_ALIGN, physical_address);
}

uint32_t kmalloc_ap(uint32_t size, uint32_t *physical_address)
{
  return _kmalloc(size, ALIGN_PAGE, physical_address);
}
