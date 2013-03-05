#ifndef _KSTREAM_H
#define _KSTREAM_H

#include <kernel/stdarg.h>
#include <kernel/util.h>

void init_kstream(void);

/* kprintf() and kvprintf produce output according to a format as described
 * below by writing output to kstream.
 *
 * RETURN VALUE
 *
 * Upon successful return, these functions return the number of characters
 * printed (excluding the null byte used to end output to strings).
 *
 * The functions snprintf() and vsnprintf() do not write more than size bytes
 * (including the terminating null byte ('\0')).  If the output was truncated
 * due to this limit then the return value is the number of characters
 * (excluding the terminating null byte) which would have been written to the
 * final string if enough space had been available.  Thus, a return value of
 * size or more means that the output was truncated.
 *
 * If an output error is encountered, a negative value is returned.
 *
 * FORMAT OF THE FORMAT STRING
 *
 * The format string is a character string, beginning and ending in its initial
 * shift state, if any.  The format string is composed of zero or more
 * directives: ordinary characters (not %), which are copied unchanged to the
 * output stream; and conversion specifications, each of which results in
 * fetching zero or more subsequent arguments.  Each conversion specification
 * consists of the character %, followed by a conversion character.
 *
 * The arguments must correspond properly (after type promotion) with the
 * conversion specifier. By default, the arguments are used in the order given,
 * where each '*' and each conversion specifier asks for the next argument (and
 * it is an error if insufficiently many arguments are given).
 *
 * THE CONVERSION CHARACTER
 *
 * A character that specifies the type of conversion to be applied.  The
 * conversion specifiers and their meanings are:
 *
 * d, i  The int argument is converted to signed decimal notation.
 *
 * f, F  The double argument is rounded and converted to decimal notation in the
 *       style [-]ddd.ddd.
 *
 * c     The int argument is converted to an unsigned char, and the resulting
 *       character is written.
 *
 * s     The const char * argument is expected to be a pointer to an array of
 *       character type (pointer to a string). Characters from the array are
 *       written up to (but not including) a terminating null byte ('\0').
 *
 * %     A '%' is written. No argument is converted. The complete conversion
 *       specification is '%%'.
 */
int printf(const char *format, ...);
int vprintf(const char *format, va_list ap);
int nprintf(size_t size, const char *format, ...);
int nvprintf(size_t size, const char *format, va_list ap);

#ifdef DEBUG
# define kdebug(...)   printf(__VA_ARGS__)
#else
# define kdebug(...)
#endif /* DEBUG */

/* Define this for stdio debugging. */
#define STDIO_DEBUG 1

#ifdef STDIO_DEBUG
# define stdio_debug(...) {				\
		kdebug("%s:%d, %s() ",		\
		       __FILE__, __LINE__, __func__);	\
		kdebug(__VA_ARGS__);			\
	}
#else
# define stdio_debug(f, ...) /**/
#endif

#endif /* _KSTREAM_H */
