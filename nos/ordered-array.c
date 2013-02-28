#include <ordered-array.h>

#include <assert.h>
#include <kheap.h>
#include <kstream.h>
#include <string.h>
#include <util.h>

sint8_t default_predicate(type_t a, type_t b)
{
	return (a > b) ? 1 : 0;
}

/* Constructors and destructors. */
struct ordered_array_s ordered_array_new(uint32_t max_size,
                                         predicate_t predicate)
{
	struct ordered_array_s array;

	array.data = kcreate(type_t, max_size);
	array.size = 0;
	array.max_size = max_size;
	array.predicate = predicate;

	/* Zero all array data. */
	memset((void *)array.data, 0x0, (sizeof(type_t) * max_size));

	return array;
}

struct ordered_array_s ordered_array_place(void *address, uint32_t max_size,
                                           predicate_t predicate)
{
	struct ordered_array_s array;

	array.data = (type_t*)address;
	array.size = 0;
	array.max_size = max_size;
	array.predicate = predicate;

	/* Zero all array data. */
	memset((void *)array.data, 0x0, (sizeof(type_t) * max_size));

	return array;
}

void ordered_array_destroy(struct ordered_array_s *array)
{
	kfree((void*)array->data);
}

void ordered_array_insert(struct ordered_array_s *array, type_t item)
{
	uint32_t i = 0;

	assert(array->predicate);

	/* Traverse along the list while item < array->data[i]. */
	while (i < array->size && !array->predicate(array->data[i], item)) {
		i++;
	}

	if (i < array->size) {
		type_t old_value = array->data[i];

		array->data[i] = item;

		/* Shift everything back accordingly. */
		while (i < array->size) {
			type_t shift_value;

			i++;
			shift_value = array->data[i];
			array->data[i] = old_value;
			old_value = shift_value;
		}

		/* Bump the array size. */
		array->size++;
	} else {
		/* If we are at the end of the list, simply tag it on the end. */
		array->data[array->size++] = item;
	}
}

type_t ordered_array_lookup_index(struct ordered_array_s *array,
				  uint32_t index)
{
	if (index < array->size) {
		return array->data[index];
	} else {
		k_debug("Attempt to access index %d of array[%d]",
			index, array->size);
		return 0;
	}
}

void ordered_array_remove_index(struct ordered_array_s *array,
				uint32_t index)
{
	if (index > array->size) {
		k_debug("Attempt to remove index %d of array[%d]", index,
			array->size);
		return;
	}

	/* Bump our array elements down. */
	while (index < array->size) {
		array->data[index] = array->data[index+1];
		index++;
	}

	array->size--;
}
