#include "list.h"

#include <stddef.h>

#include "system.h"
// For malloc calls.

struct element* list_new(void* init_val)
{
	struct element* newlist = malloc(sizeof(struct element));
	newlist->value = init_val;
	newlist->next = NULL;
	newlist->prev = NULL;
	return newlist;
}

void list_delete(struct element* list)
{
	list = get_first(list);
	
	while (list->next != NULL) // Loop until there are no more elements.
	{
		list = list->next; // If there's only one item, remove it and return.
		list_remove(list->prev);
	}
	list_remove(list); // And remove the last or only element.
}

void list_add(struct element* list, void* val)
{
	list = get_last(list); // Set list to the last element.
		
	struct element* newelem = malloc(sizeof(struct element)); // Allocate space for the element.
	newelem->value = val; // Copy the given value.
	newelem->prev = list; // Since list_add appends values, the previous pointer is the last element in the list.
	newelem->next = NULL; // And make sure the next element is marked as null.
}

void list_addat(struct element* list, void* val, int index)
{
	struct element* prev = get_at(list, index); // First, get a pointer to the indexed item.
	struct element* next = prev->next; // And a pointer to the item after that.
	
	// Allocate and assign.
	struct element* newelem = malloc(sizeof(struct element));
	newelem->value = val;
	
	// Update pointers.
	newelem->prev = prev; // First for the new element.
	newelem->next = next;
	
	prev->next = newelem; // Then for the surrounding elements.
	next->prev = newelem;
}

void list_addfirst(struct element* list, void* val)
{
	list = get_first(list);
		
	struct element* newelem = malloc(sizeof(struct element));
	newelem->value = val;
	newelem->prev = NULL; // Exactly the same as list_add, but prev
	newelem->next = list; // is marked NULL while next is assigned.
}

void list_remove(struct element* item)
{
	// Get pointers to the elements around item.
	struct element* prev = item->prev;
	struct element* next = item->next;

	// Splice item out of the linked list.
	prev->next = next;
	next->prev = prev;
	
	free(item); // And lastly, free the actual element.
}

void list_removeat(struct element* list, int index)
{
	list_remove(get_at(list, index));
}

void list_removelast(struct element* list)
{
	list_remove(get_last(list));
}

int list_count(struct element* list)
{
	int c = 1; // Initial list element.
	struct element* ptr;
	ptr = list;
	
	// First, find how many elements exist before the given element.
	while (ptr->prev != NULL)
	{
		ptr = ptr->prev; 
		c++; // Offset the pointer to the above element and increment the counter.
	}
	
	ptr = list; // Once that's done, reset the pointer back to the given element.
	
	// And then count the elements after the given list.
	while (ptr->next != NULL)
	{
		ptr = ptr->next;
		c++;
	}
	
	return c;
}

struct element* get_first(struct element* list)
{
	while (list->prev != NULL)
		list = list->prev;	
	return list;
}

struct element* get_last(struct element* list)
{
	while (list->next != NULL)
		list = list->next;
	return list;
}

struct element* get_at(struct element* list, int index)
{
	if (index < 0 || index > list_count(list))
		return NULL; // Error check, make sure index isn't negative or out of bounds.

	int i = 0;
	int max = list_count(list);
	list = get_first(list); // Set the list pointer to the first value.
	
	while (i != index)
	{
		i++; // Increment both the element pointer and the index counter until both are equal.
		list = list->next;
		
		if (i > max || list == NULL)
			return NULL; // Check for out of bounds or overflow, and return NULL.
	}
	return list;
}