#ifndef LUNA_EV_BUS_H
#define LUNA_EV_BUS_H

#include "luna_ev.h"
#include "luna_obj.h"
#include "luna_ps.h"
#include "luna_slist.h"

struct bus_sub_node {
	struct s_node node;
	struct core_obj *obj;
};

struct bus_ev_node {
	struct s_node node;
	struct core_ev ev;
	struct s_node *sub_list;
};

struct core_ev_bus {
	struct core_ps ps;
	struct s_node *ev_list;
};

void luna_ev_bus_init(struct core_ev_bus *bus);

void luna_ev_bus_ev_register(struct core_ev_bus *bus, struct core_ev ev);
void luna_ev_bus_ev_unregister(struct core_ev_bus *bus, struct core_ev ev);
void luna_ev_bus_destroy(struct core_ev_bus *bus);

#endif

#ifdef LUNA_EV_BUS_IMPLEMENTATION

#include <stdlib.h>
#include <string.h>

static struct bus_ev_node *find_ev_node(struct s_node *list, struct core_ev *ev)
{
	struct s_node *p;
	struct s_node *tmp;
	luna_slist_foreach_safe(p, tmp, list) {
		struct bus_ev_node *n = (struct bus_ev_node *)p;
		if (n->ev.sig == ev->sig)
			return n;
	}
	return NULL;
}

static void luna_ev_bus_subscribe(struct core_ps *ps, void *topic, void const *user)
{
	LUNA_ASSERT(ps);
	LUNA_ASSERT(topic);
	LUNA_ASSERT(user);

	struct core_ev_bus *bus = (struct core_ev_bus *)ps;
	struct core_ev *ev = (struct core_ev *)topic;
	struct core_obj *obj = (struct core_obj *)user;

	struct bus_ev_node *ev_node = find_ev_node(bus->ev_list, ev);
	if (!ev_node) return;

	struct s_node *p;
	struct s_node *tmp;
	luna_slist_foreach_safe(p, tmp, ev_node->sub_list) {
		struct bus_sub_node *s = (struct bus_sub_node *)p;
		if (s->obj == obj) {
			return;
		}
	}

	struct bus_sub_node *new_sub = LUNA_MALLOC(sizeof(*new_sub));
	LUNA_ASSERT(new_sub);
	new_sub->obj = obj;
	luna_slist_append(&ev_node->sub_list, &new_sub->node);
}

static void luna_ev_bus_unsubscribe(struct core_ps *ps, void *topic, void const *user)
{
	LUNA_ASSERT(ps);
	LUNA_ASSERT(topic);
	LUNA_ASSERT(user);

	struct core_ev_bus *bus = (struct core_ev_bus *)ps;
	struct core_ev *ev = (struct core_ev *)topic;
	struct core_obj *obj = (struct core_obj *)user;

	struct bus_ev_node *ev_node = find_ev_node(bus->ev_list, ev);
	if (!ev_node) return;

	struct s_node *p;
	struct s_node *tmp;
	luna_slist_foreach_safe(p, tmp, ev_node->sub_list) {
		struct bus_sub_node *s = (struct bus_sub_node *)p;
		if (s->obj == obj) {
			luna_slist_remove(&ev_node->sub_list, p);
			LUNA_FREE(s);
			return;
		}
	}
}

static void luna_ev_bus_publish(struct core_ps *ps, void *topic)
{
	LUNA_ASSERT(ps);
	LUNA_ASSERT(topic);

	struct core_ev_bus *bus = (struct core_ev_bus *)ps;
	struct core_ev *ev = (struct core_ev *)topic;
	struct bus_ev_node *ev_node = find_ev_node(bus->ev_list, ev);
	if (!ev_node) return;

        luna_ev_ref(ev);
	struct s_node *p;
	struct s_node *tmp;
	luna_slist_foreach_safe(p, tmp, ev_node->sub_list) {
		struct bus_sub_node *sub = (struct bus_sub_node *)p;
		luna_obj_ev_post(sub->obj, ev);
	}
        luna_ev_gc(ev);
}

void luna_ev_bus_ev_register(struct core_ev_bus *bus, struct core_ev ev)
{
	LUNA_ASSERT(bus);

	if (find_ev_node(bus->ev_list, &ev)) return;

	struct bus_ev_node *new_node = LUNA_MALLOC(sizeof(*new_node));
	LUNA_ASSERT(new_node);

	memset(new_node, 0, sizeof(*new_node));
	new_node->ev = ev;
	new_node->sub_list = NULL;

	luna_slist_append(&bus->ev_list, &new_node->node);
}

void luna_ev_bus_ev_unregister(struct core_ev_bus *bus, struct core_ev ev)
{
	LUNA_ASSERT(bus);

	struct s_node *p;
	struct s_node *tmp;
	luna_slist_foreach_safe(p, tmp, bus->ev_list) {
		struct bus_ev_node *entry = (struct bus_ev_node *)p;
		if (entry->ev.sig == ev.sig) {
			/* Release subscribers. */
			struct s_node *sub = entry->sub_list;
			while (sub) {
				struct s_node *tmp = sub;
				sub = sub->next;
				LUNA_FREE(tmp);
			}
			/* Release ev. */
			luna_slist_remove(&bus->ev_list, p);
			LUNA_FREE(entry);
			return;
		}
	}
}

void luna_ev_bus_destroy(struct core_ev_bus *bus)
{
	LUNA_ASSERT(bus);

	struct s_node *p;
	struct s_node *tmp;
	luna_slist_foreach_safe(p, tmp, bus->ev_list) {
		struct bus_ev_node *entry = (struct bus_ev_node *)p;
		/* Release subscribers. */
		struct s_node *sub = entry->sub_list;
		while (sub) {
			struct s_node *node = sub;
			sub = sub->next;
			LUNA_FREE(node);
		}
		/* Release ev. */
		luna_slist_remove(&bus->ev_list, p);
		LUNA_FREE(entry);
	}
	bus->ev_list = NULL;
}

void luna_ev_bus_init(struct core_ev_bus *bus)
{
	LUNA_ASSERT(bus);

	struct core_ps_impl impl = {
		.attach = luna_ev_bus_subscribe,
		.detach = luna_ev_bus_unsubscribe,
		.publish = luna_ev_bus_publish,
	};
	luna_ps_init(&bus->ps, &impl);
}

#endif
