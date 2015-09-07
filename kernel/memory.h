// Heap memory system, malloc() and the like.

#ifndef KERNEL_MEMORY
#define KERNEL_MEMORY

#include <stddef.h>
#include <stdbool.h>

#define HEAP_START      0x00100000  // Define the starting position of the heap. Normally, at the beginning of highmemory.
#define HEAP_PTR        ((struct memory_header*)HEAP_START) // A pointer to the first memory header on the heap.
#define HEADER_SIZE     sizeof(struct memory_header) // The size of a memory header.
#define SPLIT_THRESHOLD 4 // How many free bytes a block must have to be worth splitting.

struct memory_header
{
    void* next_ptr;
    void* prev_ptr;
    size_t block_size;
    int is_free;
} __attribute__ ((packed));

//
// Main memory functions:
//

void memory_init(int heapsize);                             // Size to initialize the primary heap, in kb.
void *kmalloc(size_t size);                                 // Allocate a block of memory.
void *krealloc(void *ptr, size_t newsz);                    // Reallocate a block of memory to a new size.
void kfree(void *ptr);                                      // Free a block of memory back to the heap.
void kmemcpy(void *dest, void *src, size_t sz);             // Copy a block of memory to somewhere else.
void kmemset(void *dest, char c, size_t sz);                // Set every byte in a block of memory to c.

//
// Statistics:
//

int total_blocks(); // Return how many blocks are in the kernel heap, both used and free.
int free_blocks();  // Return the number of currently free blocks in the heap.
int used_blocks();  // Return the number of currently used blocks in the heap.
int free_memory();  // Return how many NON CONTIGUOUS (AKA not all usable in one chunk) bytes are marked free.
int used_memory();  // Return how many bytes are currently marked as used.

//
// Internal memory operations (don't touch):
//

// Splits a memory block in two, at size. Both blocks must not be in use.
void splitblock(struct memory_header* block, size_t size);

// Merges two free memory blocks. Note that they MUST be marked free, or nothing happens.
void mergeblock(struct memory_header* block1, struct memory_header* block2);

void *to_data(struct memory_header* block);     // Converts a memory header's address to the data block pointer.
struct memory_header* to_header(void* data);    // Converts a data block pointer to a pointer of the header.

#endif