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
}

void list_delete()
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
}