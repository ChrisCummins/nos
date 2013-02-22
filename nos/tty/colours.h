#ifndef _TTY_COLOURS
#define _TTY_COLOURS

/* The TTY has 16 possible colours which can be assigned as foreground and
 * background colours. */
enum tty_colour_e {
  TTY_COLOUR_BLACK         = 0x0,
  TTY_COLOUR_DARK_GREY     = 0x8,
  TTY_COLOUR_BLUE          = 0x1,
  TTY_COLOUR_LIGHT_BLUE    = 0x9,
  TTY_COLOUR_GREEN         = 0x2,
  TTY_COLOUR_LIGHT_GREEN   = 0xA,
  TTY_COLOUR_CYAN          = 0x3,
  TTY_COLOUR_LIGHT_CYAN    = 0xB,
  TTY_COLOUR_RED           = 0x4,
  TTY_COLOUR_LIGHT_RED     = 0xC,
  TTY_COLOUR_MAGENTA       = 0x5,
  TTY_COLOUR_LIGHT_MAGNETA = 0xD,
  TTY_COLOUR_BROWN         = 0x6,
  TTY_COLOUR_LIGHT_BROWN   = 0xE,
  TTY_COLOUR_LIGHT_GREY    = 0x7,
  TTY_COLOUR_WHITE         = 0xF
};

#endif /* _TTY_COLOURS */
