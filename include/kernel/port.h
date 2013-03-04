#ifndef _PORT_H
#define _PORT_H

#include <ports/pic.h>
#include <ports/pit.h>
#include <ports/tty.h>

#include <kernel/types.h>

typedef unsigned short port_t;

void out_byte(port_t port, uint8_t value);
uint8_t in_byte(port_t port);
uint16_t in_word(port_t port);

#endif /* _PORT_H */
