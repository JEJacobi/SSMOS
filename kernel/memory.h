// Heap memory system, malloc() and the like.

#ifndef KERNEL_MEMORY
#define KERNEL_MEMORY

#include <stddef.h>
#include <stdbool.h>

#define HEAP_START		0x00100000	// Define the starting position of the heap. Normally, at the beginning of highmemory.
#define HEAP_PTR		((struct memory_header*)HEAP_START) // A pointer to the first memory header on the heap.
#define HEADER_SIZE		sizeof(struct memory_header) // The size of a memory header.
#define SPLIT_THRESHOLD	4 // How many free bytes a block must have to be worth splitting.

struct memory_header
{
	void* next_ptr;
	void* prev_ptr;
	int block_size;
	int is_free;
} __attribute__ ((packed));

void memory_init(int heapsize);								// Size to initialize the primary heap, in kb.
void *kmalloc(size_t size);									// Allocate a block of memory.
void *krealloc(void *ptr, size_t newsz);					// Reallocate a block of memory to a new size.
void kfree(void *ptr);										// Free a block of memory back to the heap.
void kmemcpy(void *dest, void *src, size_t sz);				// Copy a block of memory to somewhere else.
void kmemset(void *dest, char c, size_t sz);				// Set every byte in a block of memory to c.

// Internal memory operations:

// Splits a memory block in two, at size. Both blocks must not be in use.
void splitblock(struct memory_header* block, size_t size);

// Merges two free memory blocks. Note that they MUST be marked free, or nothing happens.
void mergeblock(struct memory_header* block1, struct memory_header* block2);

#endif