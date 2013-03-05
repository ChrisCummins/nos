#include <lib/stdio.h>

#include <kernel/tty.h>
#include <lib/string.h>

/* Defined in ./tty.c */
extern uint8_t tty_cursor_x;
extern uint8_t tty_cursor_y;

void init_kstream()
{
	init_tty();
	stdio_debug("\n");
	tty_set_colour(TTY_COLOUR_BLACK, TTY_COLOUR_WHITE);
}

int printf(const char *format, ...)
{
	va_list ap;
	int vprintf_return;

	va_start(ap, format);
	vprintf_return = vprintf(format, ap);
	va_end(ap);

	return vprintf_return;
}

int vprintf(const char *format, va_list ap)
{
	return nvprintf(strlen(format), format, ap);
}

int nprintf(size_t size, const char *format, ...)
{
	va_list ap;
	int vprintf_return;

	va_start(ap, format);
	vprintf_return = nvprintf(size, format, ap);
	va_end(ap);

	return vprintf_return;
}

int nvprintf(size_t size, const char *format, va_list ap)
{
	uint32_t i;

	if (!format) {
		return -1;
	}

	for (i = 0; i < size; i++) {
		/* Detect delimiter. */
		if (format[i] == '%') {
			if ((i + 1) < size && format[i+1] != '%') {
				char conv = format[i+1];

				if (conv == 'd' || conv == 'i') {
					int temp = va_arg(ap, int);

					tty_puti(temp);
					i++;
				}

				if (conv == 'c') {
					char temp = va_arg(ap, char);

					tty_putc(temp);
					i++;
				}

				if (conv == 's') {
					char *temp = va_arg(ap, char*);

					tty_write(temp);
					i++;
				}

				if (conv == 'h') {
					int temp = va_arg(ap, int);

					tty_puth(temp);
					i++;
				}

				if (conv == 'p') {
					uint32_t pointer = va_arg(ap, uint32_t);

					tty_puth(pointer);
					i++;
				}
			}
		} else {
			tty_putc(format[i]);
		}
	}
	return 0;
}
