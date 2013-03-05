#ifndef _ORDERED_ARRAY_H
#define _ORDERED_ARRAY_H

#include <kernel/types.h>

/* Define this for ordered array debugging. */
#define ORDERED_ARRAY_DEBUG 1

#ifdef ORDERED_ARRAY_DEBUG
# define ordered_array_debug(...) {			\
		kdebug("%s:%d, %s() ",			\
		       __FILE__, __LINE__, __func__);	\
		kdebug(__VA_ARGS__);			\
	}
#else
# define ordered_array_debug(f, ...) /**/
#endif

/* Predicates must return non-zero if the first argument is greater than the
 * second. If the second argument is less than the first, return zero. */
typedef sint8_t (*predicate_t)(type_t, type_t);

/* A default predicate. This equates to (a > b) ? 1 : 0. */
sint8_t default_predicate(type_t a, type_t b);

/* An ordered array. */
struct ordered_array {
  type_t *data;          /* Pointer to array's first element. */
  uint32_t size;         /* The size (length) of the array.   */
  uint32_t max_size;     /* size cannot exceed this value.    */
  predicate_t predicate; /* Predicate function.               */
};

/* Construct an ordered array, allocating space for it. */
struct ordered_array ordered_array_new(uint32_t max_size,
				       predicate_t predicate);

/* Construct an ordered array, using 'address' as the data starting point. This
 * means that you can control the location of the array. */
struct ordered_array ordered_array_place(void *address, uint32_t max_size,
					 predicate_t predicate);

void ordered_array_destroy(struct ordered_array *array);

/* List functions. */
void ordered_array_insert(struct ordered_array *array, type_t item);
type_t ordered_array_lookup_index(struct ordered_array *array, uint32_t index);
void ordered_array_remove_index(struct ordered_array *array, uint32_t index);

#endif /* _ORDERED_ARRAY_H */
