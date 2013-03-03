#include <sched/task.h>

#include <kheap.h>
#include <paging.h>
#include <string.h>

#define INIT_STACK_LOCATION (void*)0xE0000000
#define INIT_STACK_SIZE 0x2000

/* We use this in context_switch() to detect situations where we have just
 * switched tasks. */
#define TASK_SWITCH_DUMMY_VALUE 0x12345

/* Defined in ../paging.c */
extern struct page_directory_s *kernel_directory;
extern struct page_directory_s *current_directory;
extern void alloc_frame(struct page_s *p, int is_kernel, int is_writeable);
extern uint32_t read_eip(void);

/* Defined in ../main.c */
extern uint32_t initial_esp;

volatile struct task_s *current_task;

/* The start of the task linked list. */
volatile struct task_s *ready_queue;

/* The next available PID. */
uint32_t next_pid = 1;

void init_tasking ()
{
	/* We can't afford to be interrupted. */
	__asm volatile("cli");

	/* Relocate the stack so we know where it is. */
	stack_mv(INIT_STACK_LOCATION, INIT_STACK_SIZE);

	/* Initialise the kernel task as the first task. */
	current_task = kcreate(struct task_s, 1);
	current_task->pid = next_pid++;
	current_task->esp = 0;
	current_task->ebp = 0;
	current_task->eip = 0;
	current_task->pde = current_directory;
	current_task->next = 0;

	/* Initialise our task queue. */
	ready_queue = current_task;

	__asm volatile("sti");
}

void context_switch()
{
	uint32_t esp, ebp, eip;

	if (!current_task) {
		/* Nothing to switch to, tasking uninitialised. */
		return;
	}

	/* Get the esp, ebp and eip values of the currently executing task
	 * before we modify their values. */
	__asm volatile("mov %%esp, %0" : "=r"(esp));
	__asm volatile("mov %%ebp, %0" : "=r"(ebp));

	eip = read_eip();

	/* If the instruction pointer contains our dummy value, then we have
	 * just switched tasks, and must return immediately. */
	if (eip == TASK_SWITCH_DUMMY_VALUE) {
		return;
	}

	/* If we have reached this code then we have not switched tasks, so save
	 * esp, ebp and eip and perform the context switch. */
	current_task->eip = eip;
	current_task->esp = esp;
	current_task->ebp = ebp;

	current_task = current_task->next;
	k_debug("context_switch()!");

	/* The last task in our ready queue always contains a null pointer, so
	 * if that is the case, start at the beginning of the list again. */
	if (!current_task) {
		current_task = ready_queue;
	}

	eip = current_task->eip;
	esp = current_task->esp;
	ebp = current_task->ebp;

	/* Notify the MM that we've changed to a different PDE. */
	current_directory = current_task->pde;

	k_debug("EIP: %h", eip);


	/* TODO: Replace 0x12345 with macro. */
	/* 1) Disable interrupts.
	 * 2) Load the new EIP in ECX.
	 * 3) Load the stack and base pointers from the task structure.
	 * 4) Load a dummy value into EAX so that we know we've just performed
	 *    a context switch.
	 * 5) Re-enable interrupts.
	 * 6) Jump to the location in ECX.
	 */
      	/* __asm volatile("         \ */
        /*      cli;			 \ */
        /*      mov %0, %%ecx;		 \ */
        /*      mov %1, %%esp;		 \ */
        /*      mov %2, %%ebp;		 \ */
        /*      mov %3, %%cr3;		 \ */
        /*      mov $0x12345, %%eax;	 \ */
        /*      sti;			 \ */
	/* jmp *%%ecx" : : "r"(eip), "r"(esp), "r"(ebp), "r"(current_directory->directory_address)); */
}

int fork()
{
	/* struct task_s *parent_task, *new_task, *queued_task; */
	/* struct page_directory_s *pde; */
	/* uint32_t eip; */

	/* __asm volatile("cli"); */

	/* /\* Save a pointer to the current process' task. *\/ */
	/* parent_task = (struct task_s *)current_task; */

	/* /\* Clone the address space. *\/ */
	/* pde = clone_directory(current_directory); */

	/* /\* Create a new process. *\/ */
	/* new_task = kcreate(struct task_s, 1); */

	/* new_task->pid = next_pid++; */
	/* new_task->esp = 0; */
	/* new_task->ebp = 0; */
	/* new_task->eip = 0; */
	/* new_task->pde = pde; */
	/* new_task->next = 0; */

	/* /\* Add the newly created task to the end of the ready queue. *\/ */
	/* queued_task = (struct task_s*)ready_queue; */
	/* while (queued_task->next) { */
	/* 	/\* Iterate along the ready queue. *\/ */
	/* 	queued_task = queued_task->next; */
	/* } */

	/* /\* We now have the last task in the ready queue, so append the newly */
	/*  * created task to the list. *\/ */
	/* queued_task->next = new_task; */

	/* /\* Get the entry point for the new process. *\/ */
	/* eip = read_eip(); */

	/* /\* Here we must distinguish between the parent and child task. *\/ */
	/* if (current_task == parent_task) { */
	/* 	uint32_t esp; */
	/* 	uint32_t ebp; */

	/* 	/\* We are the parent task, so set up the esp, ebp and eip for */
	/* 	 * the child task. *\/ */
	/* 	__asm volatile("mov %%esp, %0" : "=r"(esp)); */
	/* 	__asm volatile("mov %%ebp, %0" : "=r"(ebp)); */

	/* 	new_task->esp = esp; */
	/* 	new_task->ebp = ebp; */
	/* 	new_task->eip = eip; */

	/* 	__asm volatile("sti"); */

	/* 	/\* Return the PID of the child task. *\/ */
	/* 	return new_task->pid; */
	/* } else { */
	/* 	/\* We are the child task, so return nothing. *\/ */
	/* 	return 0; */
	/* } */
}

void stack_mv(void *dst, size_t size)
{
	uint32_t i, pd_address, old_esp, old_ebp, offset, new_esp, new_ebp;

	/* Allocate space for the new stack. */
	for (i = (uint32_t)dst; i >= ((uint32_t)dst - size); i -= PAGE_SIZE) {
		/* General-purpose stack is in user mode and is writable. */
		alloc_frame(get_page(i, 1, current_directory), 0, 1);
	}

	/* We've changed a page table, so we need to inform the processor that a
	 * mapping has changed. Flush the TLB by reading and writing the PD
	 * address again.  */
	__asm volatile("mov %%cr3, %0" : "=r" (pd_address));
	__asm volatile("mov %0, %%cr3" : : "r" (pd_address));

	/* Read old ESP and EBP from registers. */
	__asm volatile("mov %%esp, %0" : "=r" (old_esp));
	__asm volatile("mov %%ebp, %0" : "=r" (old_ebp));

	/* Get the offset between the old and new stack address. */
	offset = (uint32_t)dst - initial_esp;

	/* Get the new ESP and EBPs. */
	new_esp = old_esp + offset;
	new_ebp = old_ebp + offset;

	/* Copy the stack. */
	memcpy((void *)new_esp, (void *)old_esp, initial_esp - old_esp);

	/* Back trace through the original stack, copying new values into the
	 * new stack. */
	for (i = (uint32_t)dst; i > (uint32_t)dst - size; i -= 4) {
		uint32_t tmp = *(uint32_t *)i;

		/* If the value of tmp is inside the range of the old stack,
		 * assume it is a base pointer and remap it. This will
		 * unfortunately remap ANY value in this range, whethere they
		 * are base pointers or not. */
		if ((old_esp < tmp) && (tmp < initial_esp)) {
			uint32_t *tmp2;

			tmp += offset;
			tmp2 = (uint32_t *)i;
			*tmp2 = tmp;

		}
	}

	/* Change the stack. */
	__asm volatile("mov %0, %%esp" : : "r" (new_esp));
	__asm volatile("mov %0, %%ebp" : : "r" (new_ebp));
}

int getpid()
{
	return current_task->pid;
}
