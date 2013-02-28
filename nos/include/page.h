
#define PAGE_FRAME_ADDRESS_SHIFT 12
#define PAGE_AVAILABLE_SHIFT     9
#define PAGE_D_SHIFT             6
#define PAGE_A_SHIFT             5
#define PAGE_U_SHIFT             2
#define PAGE_W_SHIFT             1
#define PAGE_P_SHIFT             0

/* The page fault interrupt provides an error code with diagnostic information
 * encoded within it. */

/* If set, the fault occured during an instruction fetch. */
#define PAGE_FAULT_INSTRUCTION_FETCH_SHIFT 4

/* Is set, the fault was caused by reserved bits being overwritten. */
#define PAGE_FAULT_RESERVED_ACCESS_SHIFT   3

/* If set, the CPU was running in user-mode when it was interrupted, else
 * kernel-mode. */
#define PAGE_FAULT_INTERRUPT_SHIFT         2

/* If set, the operation that caused the fault was a write, else a read. */
#define PAGE_FAULT_WRITE_SHIFT             1

/* If set, the fault was not because the page wasn't present. */
#define PAGE_FAULT_P_SHIFT                 0
