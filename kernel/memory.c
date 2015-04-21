#include "memory.h"

#include <stddef.h>
#include <stdbool.h>

#include "graphics.h" // TEMP
#include "output.h" // TEMP

//
//	Kernel heap memory manager.
//

void memory_init(int heapsize)
{
	// Create a root memory header at HEAP_START.
	HEAP_PTR->next_ptr = NULL; // Block pointers start at null as there are no blocks yet.
	HEAP_PTR->prev_ptr = NULL;
	HEAP_PTR->block_size = (heapsize * 1024) - HEADER_SIZE; // The initial free block contains all himem minus this header.
	HEAP_PTR->is_free = true; // And lastly, mark it as free.
}

void *kmalloc(size_t size)
{
	struct memory_header* newptr = HEAP_PTR;
	
	if (newptr->next_ptr == NULL && newptr->is_free == true)
	{
		// If nothing has been allocated yet, just split off a chunk from the empty heap.
		splitblock(newptr, size);
		return newptr + HEADER_SIZE; // And return the effective address.
	}
	
	// Loop until a block is found which is both bigger than the required size and marked free.
	while (newptr->block_size < size || newptr->is_free == false)
	{
		if (newptr->next_ptr == NULL)
			return NULL; // No more blocks to try, return NULL.
		newptr = newptr->next_ptr;
	}
	
	// Now that a block has been found, is the remaining space worth splitting into a new block?
	if (newptr->block_size >= size + HEADER_SIZE + SPLIT_THRESHOLD)
	{
		splitblock(newptr, size); // If so, split the block and return the address to the first.
		
		kprintnum( // TEMP
			get_position(0, 10),
			(int)size,
			get_color(LIGHT_GREEN, BLACK),
			16);
		
		return newptr + HEADER_SIZE; // And return the effective address.
	}
	else // Otherwise, just mark this block as used and pass the effective address.
	{
		newptr->is_free = false;
		return (void*)(newptr + HEADER_SIZE);
	}
}

void *krealloc(void *ptr, size_t newsz)
{
	return NULL;
}

void kfree(void *ptr)
{
	// TODO: Need mergeblock().
}

void splitblock(struct memory_header* block, size_t size)
{
	if (size + HEADER_SIZE > block->block_size || block->is_free == false)
	{
		kprint(
			get_position(0, 22),
			"can't split",
			get_color(LIGHT_GREEN, BLACK));
		kprintnum(
			get_position(0, 23),
			block->block_size,
			get_color(LIGHT_GREEN, BLACK),
			16);
		return; // Error check, make sure block has enough space and is not already in use.
	}
	
	// Split the block at size.
	struct memory_header* newblock = block + HEADER_SIZE + size + 1; // TODO: +1 needed?
	newblock->is_free = true; // Mark as free.
	newblock->prev_ptr = block; // Set the previous block as the pre-split block.
	newblock->next_ptr = block->next_ptr; // Set the next block to the old block's next block.
	newblock->block_size = block->block_size - (size + HEADER_SIZE); // Update the size,
	
	block->next_ptr = newblock; // Next, update old block's pointer to the now-next block.
	block->block_size = size; 
	block->is_free = false; // Lastly, update the size and mark as used.
}

void mergeblock(struct memory_header* block1, struct memory_header* block2)
{
	if (block1->is_free == false || block2->is_free == false)
		return; // Error check, all blocks must be marked free to be merged.
		
	
}

//
//	Standard C-library basic memory functions.
//

void kmemcpy(void *dest, void *src, size_t sz)
{
	int i;
	
	for (i = 0; i < sz; i++)
	{
		((char*)dest)[i] = ((char*)src)[i];
	}
}

void kmemset(void *dest, char c, size_t sz)
{
	int i;
	
	for (i = 0; i < sz; i++)
	{
		*((char*) dest + i) = c; //Dereference the destination pointer + offset and assign the value.
	}
}