// Standard library for a basic linked list.

#ifndef LIB_LIST
#define LIB_LIST
#include <stddef.h>

struct element // A doubly-linked list node.
{
	void* value;			// The value, unknown to the actual list.
	struct element* next;	// The next element in a list, if it's NULL, this is the last element.
	struct element* prev;	// The previous element in a list, if it's NULL, this is the first element.
};

//
// INIT:
//
struct element* list_new(void* init_val);	// Construct a new linked list.
void list_delete(struct element* list);		// And delete it, freeing up all space used by the list (not the data).

//
// DATA:
//
void list_add				(struct element* list, void* val);					// Add an element to the end of the list.
void list_addat				(struct element* list, void* val, int index);		// Add an element after index.
void list_addfirst			(struct element* list, void* val);					// Add an element before the first.
void list_remove			(struct element* item);								// Remove element item from the list.
void list_removeat			(struct element* list, int index);					// Remove an element at index and update.
void list_removelast		(struct element* list);								// Remove the last element.
int list_count				(struct element* list);								// Get how many nodes are in the list.

void list_sort				(struct element* list,								// Sort linked list, given 
							int (*compare)(const void *, const void *));		// comparison function compare().

// Note, removing items from a list DOES NOT FREE what the value pointer is pointing to.

struct element* get_first	(struct element* list);				// Return a pointer to the first element.
struct element* get_last	(struct element* list);				// Return a pointer to the last element.
struct element* get_at		(struct element* list, int index);	// Return a pointer to list[index].

#endif