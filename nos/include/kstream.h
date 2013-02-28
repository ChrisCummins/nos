#ifndef _KSTREAM_H
#define _KSTREAM_H

#include <stdarg.h>
#include <util.h>

enum kstream_message_e {
  KMESSAGE_DEBUG,
  KMESSAGE_PRINT,
  KMESSAGE_WARNING,
  KMESSAGE_CRITICAL
};

enum kstream_style_e {
  KSTREAM_NORMAL,
  KSTREAM_HIGHLIGHT,
  KSTREAM_WARNING,
  KSTREAM_CRITICAL
};

void init_kstream(void);

int kstream_message(enum kstream_message_e type, const char *format, ...);

int kstream_printf (enum kstream_style_e style, const char *format, ...);
int kstream_vprintf(enum kstream_style_e style, const char *format,
                    va_list arguments);

/* Kernel message output. */
#define k_message(...)  kstream_message(KMESSAGE_PRINT,    __VA_ARGS__)
#define k_warn(...)     kstream_message(KMESSAGE_WARNING,  __VA_ARGS__)
#define k_critical(...) kstream_message(KMESSAGE_CRITICAL, __VA_ARGS__)

#ifdef DEBUG
# define k_debug(...)   kstream_message(KMESSAGE_DEBUG,    __VA_ARGS__)
#else
# define k_debug(...)
#endif /* DEBUG */

#endif /* _KSTREAM_H */
