#ifndef _PORTS_PIC_H
#define _PORTS_PIC_H

#include <nos/port.h>

/* Port addresses. */
#define PIC_MASTER_COMMAND_PORT 0x20
#define PIC_MASTER_DATA_PORT    0x21
#define PIC_SLAVE_COMMAND_PORT  0xA0
#define PIC_SLAVE_DATA_PORT     0xA1

/* Macros for sending bytes to the PICs. */
#define PIC_MASTER_COMMAND_OUT(b) out_byte(PIC_MASTER_COMMAND_PORT, (b))
#define PIC_MASTER_DATA_OUT(b)    out_byte(PIC_MASTER_DATA_PORT,    (b))
#define PIC_SLAVE_COMMAND_OUT(b)  out_byte(PIC_SLAVE_COMMAND_PORT,  (b))
#define PIC_SLAVE_DATA_OUT(b)     out_byte(PIC_SLAVE_DATA_PORT,     (b))

#endif /* _PORTS_PIC_H */
