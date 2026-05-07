#ifndef LUNA_DLIST_H
#define LUNA_DLIST_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

struct d_node {
	struct d_node *prev;
	struct d_node *next;
};

struct d_list {
	struct d_node head;
	uint32_t count;
};

void luna_dnode_init(struct d_node *node);

void luna_dlist_init(struct d_list *list);

uint32_t luna_dlist_size(struct d_list *list);
bool luna_dlist_is_empty(const struct d_list *list);
struct d_node *luna_dlist_peek_head(struct d_list *list);

void luna_dlist_add_head(struct d_list *list, struct d_node *node);
void luna_dlist_add_tail(struct d_list *list, struct d_node *node);

void luna_dlist_delete(struct d_list *list, struct d_node *node);

#define container_of(ptr, type, member) \
			((type *)((char *)(ptr) - offsetof(type, member)))

#define luna_dlist_foreach(list, node) \
			for (node = (list)->head.next; node != &(list)->head; node = node->next)

#define luna_dlist_foreach_safe(list, node, next_node) \
			for (node = (list)->head.next, next_node = node->next; \
				node != &(list)->head; \
				node = next_node, next_node = node->next)

#endif

#ifdef LUNA_DLIST_IMPLEMENTATION


void luna_dnode_init(struct d_node *node)
{
	node->prev = node;
	node->next = node;
}

void luna_dlist_init(struct d_list *list)
{
	luna_dnode_init(&list->head);
	list->count = 0;
}

bool luna_dlist_is_empty(const struct d_list *list)
{
	return list->count == 0;
}

uint32_t luna_dlist_size(struct d_list *list)
{
	return list->count;
}

struct d_node *luna_dlist_peek_head(struct d_list *list)
{
	return (list->count) ? list->head.next : NULL;
}

void luna_dlist_add_head(struct d_list *list, struct d_node *node)
{
	node->next = list->head.next;
	node->prev = &list->head;
	list->head.next->prev = node;
	list->head.next = node;
	list->count++;
}

void luna_dlist_add_tail(struct d_list *list, struct d_node *node)
{
	node->prev = list->head.prev;
	node->next = &list->head;
	list->head.prev->next = node;
	list->head.prev = node;
	list->count++;
}

void luna_dlist_delete(struct d_list *list, struct d_node *node)
{
	node->prev->next = node->next;
	node->next->prev = node->prev;
	luna_dnode_init(node);
	if (list->count > 0) {
		list->count--;
	}
}

#endif
