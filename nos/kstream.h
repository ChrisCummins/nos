#ifndef _KSTREAM_H
#define _KSTREAM_H

#include <nos/util.h>

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

#endif /* _KSTREAM_H */
