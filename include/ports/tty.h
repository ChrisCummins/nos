#ifndef _PORTS_TTY_H
#define _PORTS_TTY_H

#include <kernel/port.h>

/* Port addresses. */
#define TTY_COMMAND_PORT 0x3D4
#define TTY_DATA_PORT    0x3D5

/* Macros for sending bytes to the display port. */
#define TTY_COMMAND_OUT(b) out_byte(TTY_COMMAND_PORT, (b))
#define TTY_DATA_OUT(b)    out_byte(TTY_DATA_PORT,    (b))

#endif /* _PORTS_TTY_H */
