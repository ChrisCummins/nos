#ifndef _KHEAP_H
#define _KHEAP_H

#include <nos/types.h>

uint32_t kmalloc   (uint32_t size);
uint32_t kmalloc_a (uint32_t size);
uint32_t kmalloc_p (uint32_t size, uint32_t *physical_address);
uint32_t kmalloc_ap(uint32_t size, uint32_t *physical_address);

#define kcreate(type,count)    (type *)kmalloc(sizeof(type) * (count))
#define kcreate_a(type,count)  (type *)kmalloc_a(sizeof(type) * (count))
#define kcreate_p(type, count, physical_address)        \
  (type *)kmalloc_p(sizeof(type) * (count), (physical_address))
#define kcreate_ap(type, count, physical_address)       \
  (type *)kmalloc_ap(sizeof(type) * (count), (physical_address))

#endif /* _KHEAP_H */
