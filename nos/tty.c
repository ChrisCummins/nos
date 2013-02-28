#include <tty.h>
#include <port.h>
#include <tty/ascii.h>
#include <util.h>

/* The display can be thought of as a two dimension array of characters,
 * arranged into rows and columns. These values dicatate the size of each. */
#define TTY_CHAR_WIDTH  80
#define TTY_CHAR_HIEGHT 25

/* Memory addresses. */
#define VGA_FRAMEBUFFER_ADDRESS 0xB8000

/* Each TTY character consists of two bytes, with bits 7:0 being an ASCII code,
 * and the higher and lower nibbles of bits 15:8 representing the background and
 * foreground colours, respectively. */
typedef uint16_t tty_char_t;

/* Blank character. */
#define TTY_CHAR_BLANK (high_byte(high_nibble(background_colour)        \
                                  | low_nibble(foreground_colour))      \
                        | low_byte(ASCII_SPACE))

/* Here is where we keep track of the cursor position. The possible values for
 * the two variables are: tty_cursor_x [0, TTY_CHAR_WIDTH],
 * tty_cursor_y [0, TTY_CHAR_HIEGHT]. */
uint8_t tty_cursor_x = 0;
uint8_t tty_cursor_y = 0;

/* Here is where we keep track of the active background and foreground
 * colours. */
static enum tty_colour_e background_colour = TTY_COLOUR_BLACK;
static enum tty_colour_e foreground_colour = TTY_COLOUR_WHITE;

/* This pointer marks the start of the framebuffer memory. */
static tty_char_t *video_memory = (tty_char_t *) VGA_FRAMEBUFFER_ADDRESS;

/* Update the position of the hardware cursor. */
static void _update_cursor_position(void)
{
	uint16_t cursor_location;

	/* Get the location of the cursor as a flat array index. */
	cursor_location = (tty_cursor_y * TTY_CHAR_WIDTH) + tty_cursor_x;

	/* Send the high cursor byte. */
	TTY_COMMAND_OUT(14);
	TTY_DATA_OUT(cursor_location >> 8);

	/* Send the low cursor byte. */
	TTY_COMMAND_OUT(15);
	TTY_DATA_OUT(cursor_location);
}

/* Initialise the TTY. */
void init_tty()
{
	tty_clear();
	tty_set_colour(TTY_COLOUR_BLACK, TTY_COLOUR_WHITE);
}

/* Clear the TTY display. */
void tty_clear()
{
	int i;

	for (i = 0; i < TTY_CHAR_WIDTH * TTY_CHAR_HIEGHT; i++) {
		video_memory[i] = TTY_CHAR_BLANK;
	}

	/* Return hardware cursor to start. */
	tty_cursor_x = 0;
	tty_cursor_y = 0;
	_update_cursor_position();
}

/* Scroll the display one line. */
void tty_scroll_line()
{
	int i;
	int j;

	/* Duplicate each line onto the previous one. */
	for (i = 1; i < (TTY_CHAR_HIEGHT - 1); i++) {
		for (j = 0; j < TTY_CHAR_WIDTH; j++) {
			video_memory[((i - 1) * TTY_CHAR_WIDTH) + j] =
				video_memory[(i * TTY_CHAR_WIDTH) + j];
		}
	}

	/* Insert a blank line at the end. Fill the new line with the background
	 * colour of the character directly above. */
	for (i = 0; i < TTY_CHAR_WIDTH; i++) {
		video_memory[(TTY_CHAR_WIDTH * (TTY_CHAR_HIEGHT - 1)) + i] = \
			low_nibble(video_memory[(TTY_CHAR_WIDTH
						 * (TTY_CHAR_HIEGHT - 2)) + i]);
	}

	/* Scroll the hardware cursor. */
	if (tty_cursor_y) {
		tty_cursor_y--;
		_update_cursor_position();
	}
}

/* Move the TTY cursor to a new position. If the cursor position is out of
 * range, the cursor will be moved to the largest possible value. */
void tty_move_cursor(uint8_t x, uint8_t y)
{
	uint8_t dest_x;
	uint8_t dest_y;

	dest_x = min(x, TTY_CHAR_WIDTH);
	dest_y = min(y, TTY_CHAR_HIEGHT);

	tty_cursor_x = dest_x;
	tty_cursor_y = dest_y;
	_update_cursor_position();
}

/* Write the given character to the TTY display. */
void tty_putc(char c)
{
	tty_char_t character;
	uint16_t *location;

	/* Create our TTY character. */
	character = high_byte(high_nibble(background_colour)        \
			      | low_nibble(foreground_colour))     \
		| low_byte(c);

	if (c == ASCII_BACKSPACE && tty_cursor_x) {
		/* In case of backspace character, get the location of the last
		 * character and mask off the low nibble. */
		location = video_memory
			+ (tty_cursor_y * TTY_CHAR_WIDTH + tty_cursor_x - 1);
		*location = *location & 0xF;
		tty_cursor_x--;
	} else if (c == '\t') {
		tty_cursor_x = (tty_cursor_x + 8) & ~(8-1);
	} else if (c == '\r') {
		tty_cursor_x = 0;
	} else if (c == '\n') {
		tty_cursor_x = 0;
		tty_cursor_y++;
	} else if (c >= ' ') {
		location = video_memory
			+ (tty_cursor_y * TTY_CHAR_WIDTH + tty_cursor_x);
		*location = character;
		tty_cursor_x++;
	}

	/* Check to see if we need to insert a newline. */
	if (tty_cursor_x >= TTY_CHAR_WIDTH) {
		tty_cursor_x = 0;
		tty_cursor_y++;
	}

	/* Check to see if we need a new line. */
	if (tty_cursor_y >= TTY_CHAR_HIEGHT) {
		tty_scroll_line();
	}

	/* Move hardware cursor. */
	_update_cursor_position();
}

#define INT32_MAX_HEX_DIGITS 8

/* Print a hexidecimal representation of a 32-bit integer to the TTY. */
void tty_puth(uint32_t h)
{
	/* Create a character buffer and point to the terminating '\0' in it. */
	static char buffer[INT32_MAX_HEX_DIGITS + 2];
	char *pointer;

	tty_nwrite("0x", sizeof("0x"));
	pointer = buffer + INT32_MAX_HEX_DIGITS + 1;

	do {
		uint8_t nibble;

		nibble = low_nibble(h);

		if (nibble < 0xA) {
			*--pointer = 0x30 + nibble;
		} else {
			*--pointer = 0x37 + nibble;
		}

		h = h >> 4;
	} while (h != 0);

	tty_write(pointer);
}

/* Print a binary representation of a 32-bit integer to the TTY. */
void tty_putb(uint32_t p)
{
	sint8_t i;

	tty_nwrite("0b", sizeof("0b"));

	/* Iterate over each nibble in the word. */
	for (i = 28; i >= 0; i-=4) {
		uint8_t nibble;

		nibble = low_nibble(p >> i);

		/* Determine the correct binary representation the nibble. */
		switch(nibble) {
		case 0x0:
			tty_nwrite("0000", sizeof("0000"));
			break;
		case 0x1:
			tty_nwrite("0001", sizeof("0001"));
			break;
		case 0x2:
			tty_nwrite("0010", sizeof("0010"));
			break;
		case 0x3:
			tty_nwrite("0011", sizeof("0011"));
			break;
		case 0x4:
			tty_nwrite("0100", sizeof("0100"));
			break;
		case 0x5:
			tty_nwrite("0101", sizeof("0101"));
			break;
		case 0x6:
			tty_nwrite("0110", sizeof("0110"));
			break;
		case 0x7:
			tty_nwrite("0111", sizeof("0111"));
			break;
		case 0x8:
			tty_nwrite("1000", sizeof("1000"));
			break;
		case 0x9:
			tty_nwrite("1001", sizeof("1001"));
			break;
		case 0xA:
			tty_nwrite("1010", sizeof("1010"));
			break;
		case 0xB:
			tty_nwrite("1011", sizeof("1011"));
			break;
		case 0xC:
			tty_nwrite("1100", sizeof("1100"));
			break;
		case 0xD:
			tty_nwrite("1101", sizeof("1101"));
			break;
		case 0xE:
			tty_nwrite("1110", sizeof("1110"));
			break;
		case 0xF:
			tty_nwrite("1111", sizeof("1111"));
			break;
		}

		/* Insert a space between each nibble. */
		tty_putc(' ');
	}
}

#define INT32_MAX_DEC_DIGITS 10

/* Print a decimal representation of a 32-bit integer to the TTY. */
void tty_putd(uint32_t d)
{
	/* Create a character buffer and point to the terminating '\0' in it. */
	static char buffer[INT32_MAX_DEC_DIGITS + 2];
	char *pointer;

	pointer = buffer + INT32_MAX_DEC_DIGITS + 1;

	do {
		*--pointer = '0' + (d % 10);
		d /= 10;
	} while (d != 0);

	tty_write(pointer);
}

void tty_puti(int i)
{
	if (i >= 0) {
		tty_putd((uint32_t)i);
	} else {
		tty_putc('-');
		tty_putd((uint32_t)-i);
	}
}

/* Write a null-terminated character string to the TTY. */
void tty_write(const char *c)
{
	sint32_t i;

	i = 0;
	while (c[i] != '\0') {
		tty_putc(c[i++]);
	}
}

/* Write a null-terminated character string to the TTY. This is a much safer
 * implementation than tty_write, as it requires a maximum length n to prevent
 * buffer overflow from non-null terminated strings. */
void tty_nwrite(const char *c, int n)
{
	int i;

	for (i = 0; i < n; i++) {
		if (c[i] != '\0') {
			tty_putc(c[i]);
		}
	}
}

/* Set a new drawing colour for the TTY. */
void tty_set_colour (enum tty_colour_e background,
                     enum tty_colour_e foreground)
{
	tty_set_background_colour(background);
	tty_set_foreground_colour(foreground);
}

/* Set a new foreground colour to be used for writing out. */
void tty_set_foreground_colour(enum tty_colour_e colour)
{
	foreground_colour = colour;
}

/* Set a new background colour to be used for writing out. */
void tty_set_background_colour(enum tty_colour_e colour)
{
	background_colour = colour;
}
