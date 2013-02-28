#include <assert.h>
#include <initrd.h>
#include <fs.h>
#include <gdt.h>
#include <idt.h>
#include <tty.h>
#include <timer.h>
#include <multiboot.h>
#include <paging.h>
#include <string.h>
#include <kstream.h>
#include <kheap.h>

/* Defined in ./kheap.c. */
extern uint32_t placement_address;

int kmain(struct multiboot_s *mboot)
{
	uint32_t initrd_location;
	uint32_t initrd_end;
	int i = 0;
	struct fs_node_s *fs_root;
	struct dirent_s *node = 0;

	init_kstream();
	init_timer(50);
	init_idt();
	init_gdt();

	assert(mboot->mods_count > 0);
	initrd_location = *((uint32_t *)mboot->mods_addr);
	initrd_end = *(uint32_t *)(mboot->mods_addr + 4);

	/* Ensure that our module does not get overwritten. */
	placement_address = initrd_end;

	init_paging();

	fs_root = init_initrd(initrd_location);

	k_message("initrd contents:");
	while ((node = fs_readdir(fs_root, i))) {
		struct fs_node_s *fsnode = fs_finddir(fs_root, node->name);

		k_message("\t%h\t%d\t/%s",
			  ((struct initrd_file_header_s *)fsnode)->offset,
			  fsnode->size, node->name);

		if (is_dir(fsnode)) {
			tty_write("/\n");
		}

		i++;
	}

	return 0;
}
