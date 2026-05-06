#ifndef LUNA_SLIST_H
#define LUNA_SLIST_H

#include <stdbool.h>
#include <stddef.h>

struct s_node {
	struct s_node *next;
};

#define luna_slist_foreach(p, head) \
		for (struct s_node *p = (head); p != NULL; p = p->next)

#define luna_slist_foreach_safe(p, tmp, head) \
		for (p = (head), tmp = p ? p->next : NULL; \
			p != NULL; \
			p = tmp, tmp = p ? p->next : NULL)


void luna_slist_append(struct s_node **head, struct s_node *node);
bool luna_slist_remove(struct s_node **head, struct s_node *node);

#endif

#ifdef LUNA_SLIST_IMPLEMENTATION

void luna_slist_append(struct s_node **head, struct s_node *node)
{
	node->next = *head;
	*head = node;
}

bool luna_slist_remove(struct s_node **head, struct s_node *node)
{
	struct s_node **n = head;
	while (*n) {
		if (*n == node) {
			*n = node->next;
			return true;
		}
		n = &(*n)->next;
	}
	return false;
}

#endif
