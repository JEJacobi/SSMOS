#include "memory.h"

#include <stddef.h>
#include <stdbool.h>

#include "debug.h"
#include "string.h"

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
	
	// Log the heap initialization.
	string* logmsg = string_new();
	string_add(logmsg, "Kernel heap initialized, ");
	string_addnum(logmsg, (heapsize * 1024) - HEADER_SIZE, 10);
	string_add(logmsg, " bytes found and allocated.");
	klog(logmsg);
}

void *kmalloc(size_t size)
{
	struct memory_header* newptr = HEAP_PTR;
	
	if (newptr->next_ptr == NULL && newptr->is_free == true)
	{
		// If nothing has been allocated yet, just split off a chunk from the empty heap.
		splitblock(newptr, size);
			asm volatile("xchg %bx, %bx");
		return to_data(newptr); // And return the effective address.
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
			asm volatile("xchg %bx, %bx");
		return to_data(newptr); // And return the effective address.
	}
	else // Otherwise, just mark this block as used and pass the effective address.
	{
		newptr->is_free = false;
			asm volatile("xchg %bx, %bx");
		return to_data(newptr);
	}
}

void *krealloc(void *ptr, size_t newsz)
{
	struct memory_header* block = to_header(ptr);
	
	if (block->block_size < newsz) // Does the caller want a bigger block?
	{
		void* newptr = kmalloc(newsz); // Allocate a new block with the desired size.
		kmemcpy(newptr, ptr, block->block_size); // Copy over the initial data.
		kfree(ptr); 		// Free the old block.
		return newptr; 	// And return a pointer to the new one.
	}
	else if (block->block_size > newsz) // Or does the caller want a smaller block?
	{
		return ptr; // TODO: Replace this with something that actually does something. Technically valid, but really wasteful.
	}
	else // If it's the same, just return the original pointer.
		return ptr;
}

void kfree(void *ptr)
{
	struct memory_header* block = to_header(ptr); // Get the pointer to the block's header.
	
	// TODO: Really, REALLY need to check and merge blocks because this is terrible. Need mergeblock() working.
	
	block->is_free = true; // TEMP
}

void splitblock(struct memory_header* block, size_t size)
{
	if (size + HEADER_SIZE > block->block_size || block->is_free == false) // TODO: Add SPLIT_THRESHOLD check.
	{
		return; // Error check, make sure block has enough space and is not already in use.
	}
	
	// Split the block at size.
	struct memory_header* newblock = (struct memory_header*)((int)(block) + HEADER_SIZE + size); // TODO: +1 needed?
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

int total_blocks()
{
	int blocks = 0;
	struct memory_header* blockptr = HEAP_PTR;
	
	while (blockptr != NULL)
	{
		blocks++;
		blockptr = blockptr->next_ptr;
	}
	return blocks;
}

int free_blocks()
{
	int blocks = 0;
	struct memory_header* blockptr = HEAP_PTR;
	
	while (blockptr != NULL)
	{
		if (blockptr->is_free == true)
			blocks++;
			
		blockptr = blockptr->next_ptr;
	}
	return blocks;
}

int used_blocks()
{
	int blocks = 0;
	struct memory_header* blockptr = HEAP_PTR;
	
	while (blockptr != NULL)
	{
		if (blockptr->is_free == false)
			blocks++;
			
		blockptr = blockptr->next_ptr;
	}
	return blocks;
}

int free_memory()
{
	int bytes = 0;
	struct memory_header* blockptr = HEAP_PTR;
	
	while (blockptr != NULL)
	{
		if (blockptr->is_free == true)
			bytes += blockptr->block_size;
			
		blockptr = blockptr->next_ptr;
	}
	return bytes;
}

int used_memory()
{
	int bytes = 0;
	struct memory_header* blockptr = HEAP_PTR;
	
	while (blockptr != NULL)
	{
		if (blockptr->is_free == false)
			bytes += blockptr->block_size;
			
		blockptr = blockptr->next_ptr;
	}
	return bytes;
}

void *to_data(struct memory_header* block)
{
	return (void*)((int)block + HEADER_SIZE);
}

struct memory_header* to_header(void* data)
{
	return (struct memory_header*)((int)data - HEADER_SIZE);
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