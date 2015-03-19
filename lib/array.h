#ifndef LIB_ARRAY
#define LIB_ARRAY
#include <stddef.h>

void sort(	void *array,									// Pointer to the start of the array.
			size_t count,									// Number of elements in the array.
			size_t size,									// Size of an element.
			int (*compare)(const void *, const void *));	// Comparison function pointer.

#endif