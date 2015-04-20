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
	
}

void list_add(struct element* list, void* val)
{

}

void list_addafter(struct element* list, void* val, int index)
{

}

void list_addbefore(struct element* list, void* val, int index)
{

}

void list_removeat(struct element* list, int index)
{

}

void list_removelast(struct element* list)
{

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