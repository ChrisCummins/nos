#ifndef _PORT_H
#define _PORT_H

#include <nos/types.h>
#include <nos/ports/pic.h>
#include <nos/ports/pit.h>
#include <nos/ports/tty.h>

typedef unsigned short port_t;

void     out_byte(port_t port, uint8_t value);
uint8_t  in_byte (port_t port);
uint16_t in_word (port_t port);

#endif /* _PORT_H */
