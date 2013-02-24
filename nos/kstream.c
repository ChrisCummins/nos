#include <nos/kstream.h>

#include <nos/string.h>
#include <nos/tty.h>

/* Some colours. */
#define KSTREAM_NORMAL_BACKGROUND    TTY_COLOUR_BLACK
#define KSTREAM_NORMAL_FOREGROUND    TTY_COLOUR_WHITE
#define KSTREAM_HIGHLIGHT_BACKGROUND KSTREAM_NORMAL_FOREGROUND
#define KSTREAM_HIGHLIGHT_FOREGROUND KSTREAM_NORMAL_BACKGROUND
#define KSTREAM_WARNING_BACKGROUND   TTY_COLOUR_BLACK
#define KSTREAM_WARNING_FOREGROUND   TTY_COLOUR_RED
#define KSTREAM_CRITICAL_BACKGROUND  TTY_COLOUR_RED
#define KSTREAM_CRITICAL_FOREGROUND  TTY_COLOUR_WHITE

/* Defined in ./tty.c */
extern uint8_t tty_cursor_x;
extern uint8_t tty_cursor_y;

static void _set_kstream_style(enum kstream_style_e style)
{
  switch (style) {
  case KSTREAM_NORMAL:
    tty_set_colour(KSTREAM_NORMAL_BACKGROUND, KSTREAM_NORMAL_FOREGROUND);
    break;
  case KSTREAM_HIGHLIGHT:
    tty_set_colour(KSTREAM_HIGHLIGHT_BACKGROUND, KSTREAM_HIGHLIGHT_FOREGROUND);
    break;
  case KSTREAM_WARNING:
    tty_set_colour(KSTREAM_WARNING_BACKGROUND, KSTREAM_WARNING_FOREGROUND);
    break;
  case KSTREAM_CRITICAL:
    tty_set_colour(KSTREAM_CRITICAL_BACKGROUND, KSTREAM_CRITICAL_FOREGROUND);
    break;
  }
}

static void _print_message_prefix(enum kstream_message_e type)
{
  switch (type) {
  case KMESSAGE_DEBUG:
    _set_kstream_style(KSTREAM_NORMAL);
    tty_write(" DEBUG    ");
    break;
  case KMESSAGE_PRINT:
    _set_kstream_style(KSTREAM_NORMAL);
    tty_write(" MSG      ");
    break;
  case KMESSAGE_WARNING:
    _set_kstream_style(KSTREAM_WARNING);
    tty_write(" WARNN    ");
    _set_kstream_style(KSTREAM_NORMAL);
    break;
  case KMESSAGE_CRITICAL:
    _set_kstream_style(KSTREAM_CRITICAL);
    tty_write(" CRITICAL ");
    _set_kstream_style(KSTREAM_NORMAL);
    break;
  }
}

void init_kstream()
{
  init_tty();
  k_message("Initialising kstream");
}

int kstream_message(enum kstream_message_e type, const char *format, ...)
{
  va_list arguments;
  int vprintf_return;

  if (!format) {
    return -1;
  }

  if (tty_cursor_x) {
    /* If we're not at the start of a line, then scroll
     * to the next one. This way, kstream messages always
     * begin at the start of a line. */
    tty_move_cursor(0x0, ++tty_cursor_y);
  }

  _print_message_prefix(type);

  va_start(arguments, format);
  vprintf_return = kstream_vprintf(KSTREAM_NORMAL, format, arguments);
  va_end(arguments);

  return vprintf_return;
}

int kstream_printf(enum kstream_style_e style, const char *format, ...)
{
  va_list arguments;
  int vprintf_return;

  va_start(arguments, format);
  vprintf_return = kstream_vprintf(style, format, arguments);
  va_end(arguments);

  return vprintf_return;
}

int kstream_vprintf(enum kstream_style_e style, const char *format, va_list arguments)
{
  uint32_t i;
  size_t format_length = strlen(format);

  if (!format) {
    return -1;
  }

  _set_kstream_style(style);

  for (i = 0; i < format_length; i++) {
    /* Detect delimiter. */
    if (format[i] == '%') {
      if ((i + 1) < format_length && format[i+1] != '%') {
        if (format[i+1] == 's') {
          char *temp = va_arg(arguments, char*);

          tty_write(temp);
          i++;
        }

        if (format[i+1] == 'd') {
          int temp = va_arg(arguments, int);

          tty_puti(temp);
          i++;
        }

        if (format[i+1] == 'h') {
          int temp = va_arg(arguments, int);

          tty_puth(temp);
          i++;
        }

        if (format[i+1] == 'p') {
          uint32_t pointer = va_arg(arguments, uint32_t);

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
