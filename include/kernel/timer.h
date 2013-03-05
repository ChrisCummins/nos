#ifndef _TIMER_H
#define _TIMER_H

#include <kernel/types.h>

/* Define this for TIMER debugging. */
#define TIMER_DEBUG 1

#ifdef TIMER_DEBUG
# define timer_debug(...) {				\
		kdebug("%s:%d, %s() ",			\
		       __FILE__, __LINE__, __func__);	\
		kdebug(__VA_ARGS__);			\
	}
#else
# define timer_debug(f, ...) /**/
#endif

void init_timer(uint32_t frequency);

#endif /* _TIMER_H */
