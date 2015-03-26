#ifndef LIB_LIST
#define LIB_LIST
#include <stddef.h>

struct element // A doubly-linked list node.
{
	void* value;			// The value, unknown to the actual list.
	struct element* next;	// The next element in a list, if it's NULL, this is the last element.
	struct element* prev;	// The previous elemnt in a list, if it's NULL, this is the first element.
};

// INIT:
struct element* list_new(void* init_val);	// Construct a new linked list.
void list_delete();							// And delete it, freeing up all space used by the list (not the data).

// DATA:
void list_add				(struct element* list, void* val);					// Add an element to the end of the list.
void list_addafter			(struct element* list, void* val, int index);		// Add an element after index.
void list_addbefore			(struct element* list, void* val, int index);		// Add an element before index.
void list_removeat			(struct element* list, int index);					// Remove an element at index and update.
void list_removelast		(struct element* list);								// Remove the last element.
int list_count				(struct element* list);								// Get how many nodes are in the list.

#endif