#ifndef _SCHED_TASK_H
#define _SCHED_TASK_H

#include <kernel/types.h>

/* The structure of a task.
 *
 *   pid  - Process ID
 *   esp  - Stack pointer.
 *   ebp  - Base pointer.
 *   eip  - Instruction pointer.
 *   pde  - Page directory.
 *   next - A pointer to the next task.
 */
struct task {
	int pid;
	uint32_t esp;
	uint32_t ebp;
	uint32_t eip;
	struct page_directory *pde;
	struct task *next;
};

void init_tasking(void);
void context_switch(void);
int fork(void);
void stack_mv(void *dst, size_t size);
int getpid(void);

#endif /* _SCHED_TASK_H */
