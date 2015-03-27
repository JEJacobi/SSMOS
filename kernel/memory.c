#include "memory.h"

#include <stddef.h>

void *heapptr;

void memory_init(int heapsize)
{
	heapptr = (void*)HEAP_START; // Basic watermark heap system.
}

void *malloc(size_t size)
{
	void *retptr = heapptr;
	heapptr += size;
	return retptr;
}

void *realloc(void *ptr, size_t newsz)
{
	// TODO: Will need memory headers working to find out how much to copy to the new block.
	return NULL;
}

void free(void *ptr)
{
	// TODO: Memory headers again.
}

void memcpy(void *dest, const void *src, size_t sz)
{
	int i;
	
	for (i = 0; i < sz; i++)
	{
		((char*)dest)[i] = ((char*)src)[i];
	}
}

void memset(void *dest, char c, size_t sz)
{
	int i;
	
	for (i = 0; i < sz; i++)
	{
		*((char*) dest + i) = c; //Dereference the destination pointer + offset and assign the value.
	}
}