#ifndef _TTY_H
#define _TTY_H

#include <types.h>
#include <tty/colours.h>

void init_tty(void);

/* Screen and cursor control. */
void tty_clear(void);
void tty_scroll_line(void);
void tty_move_cursor(uint8_t x, uint8_t y);

/* Output text. */
void tty_putc(char c);
void tty_puth(uint32_t h);
void tty_putb(uint32_t b);
void tty_putd(uint32_t d);
void tty_puti(int i);

void tty_write(const char *c);
void tty_nwrite(const char *c, int n);

void tty_set_foreground_colour(enum tty_colour_e colour);
void tty_set_background_colour(enum tty_colour_e colour);
void tty_set_colour(enum tty_colour_e background,
		    enum tty_colour_e foreground);

#endif /* _TTY_H */
