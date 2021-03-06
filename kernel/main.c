#include <fs/fs.h>
#include <fs/initrd.h>
#include <kernel/assert.h>
#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/multiboot.h>
#include <kernel/timer.h>
#include <kernel/tty.h>
#include <lib/stdio.h>
#include <lib/string.h>
#include <mm/heap.h>
#include <mm/paging.h>
#include <sched/task.h>

/* Defined in ./kheap.c. */
extern uint32_t placement_address;
uint32_t initial_esp;

int kmain(struct multiboot *mboot, uint32_t stack)
{
	uint32_t initrd_location;
	uint32_t initrd_end;
	int i = 0;
	struct fs_node *fs_root;
	struct dirent *node = 0;

	/* Get our stack pointer. */
	initial_esp = stack;

	init_kstream();
	init_idt();
	init_gdt();

	/* Initialise the PIT to 100 Hz. */
	__asm volatile("sti");
	init_timer(50);

	assert(mboot->mods_count > 0);
	initrd_location = *((uint32_t *)mboot->mods_addr);
	initrd_end = *(uint32_t *)(mboot->mods_addr + 4);

	/* Ensure that our module does not get overwritten. */
	placement_address = initrd_end;

	/* Start paging. */
	init_paging();
	init_tasking();

	fs_root = init_initrd(initrd_location);

	/* int ret = fork(); */
	/* k_message("fork() = %h, getpid() = %h", ret, getpid()); */

	__asm volatile("cli");
	printf("initrd contents:\n");
	while ((node = fs_readdir(fs_root, i))) {
		struct fs_node *fsnode = fs_finddir(fs_root, node->name);

		printf("\t%h\t%d\t/%s",
		       ((struct initrd_file_header *)fsnode)->offset,
		       fsnode->size, node->name);

		if (is_dir(fsnode)) {
			tty_putc('/');
		}

		tty_putc('\n');
		i++;
	}

	__asm volatile("sti");

	return 0;
}
