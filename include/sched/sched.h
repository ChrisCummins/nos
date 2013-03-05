#ifndef _SCHED_H
#define _SCHED_H

#include <lib/stdio.h>

/* Define this for scheduler debugging. */
#undef SCHED_DEBUG

#ifdef SCHED_DEBUG
# define sched_debug(...) {                                \
		kdebug("%s:%d, %s ",			   \
		       __FILE__, __LINE__, __func__);	   \
		kstream_printf(__VA_ARGS__);		   \
	}
#else
# define sched_debug(f, ...) /**/
#endif

#endif
