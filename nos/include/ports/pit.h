#ifndef _PORTS_PIT_H
#define _PORTS_PIT_H

#include <port.h>

/* Port addresses. */
#define PIT_CHANNEL_0_DATA 0x40
#define PIT_CHANNEL_1_DATA 0x41
#define PIT_CHANNEL_2_DATA 0x42
#define PIT_COMMAND_PORT   0x43

/* Macros for sending bytes to the PIT. */
#define PIT_COMMAND_OUT(b) out_byte(PIT_COMMAND_PORT,   (b))
#define PIT_0_DATA_OUT(b)  out_byte(PIT_CHANNEL_0_DATA, (b))
#define PIT_1_DATA_OUT(b)  out_byte(PIT_CHANNEL_1_DATA, (b))
#define PIT_2_DATA_OUT(b)  out_byte(PIT_CHANNEL_2_DATA, (b))

#endif /* _PORTS_PIT_H */
