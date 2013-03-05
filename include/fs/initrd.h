#ifndef _INITRD_H
#define _INITRD_H

#include <kernel/types.h>

struct initrd_header {
	uint32_t file_count; /* The number of files in the ramdisk image. */
};

struct initrd_file_header {
	char name[128];  /* Filename. */
	uint8_t id;      /* Identification value, used for consitency checking. */
	uint32_t offset; /* Initrd offset of the beginning of the file. */
	uint32_t size;   /* Size of the file. */
};

/* Initialise the ramdisk. It accepts as a parameter the address of the
 * multiboot module, and returns a filesystem node. */
struct fs_node *init_initrd(uint32_t location);

#endif /* _INITRD_H */
