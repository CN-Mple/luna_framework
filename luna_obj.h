/* luna_obj.h */
#ifndef LUNA_OBJ_H
#define LUNA_OBJ_H

#include "luna_rq.h"
#include "luna_ev.h"
#include "luna_fsm.h"
#include "luna_dlist.h"

#define LUNA_OBJ_CONFIG_MAX_PRIORITY    8

struct core_obj {
	struct core_fsm super;
	struct rq       mailbox;
	struct d_node   node;
	uint32_t        priority;
};

void luna_obj_init(void);

void luna_obj_add(struct core_obj *obj, uint8_t *buffer, uint32_t size, uint32_t priority);
void luna_obj_ev_post(struct core_obj *obj, struct core_ev *ev);
void luna_obj_schedule(void);

#endif

#ifdef LUNA_OBJ_IMPLEMENTATION

uint32_t luna_obj_ready_map = 0;
struct d_list luna_obj_list[LUNA_OBJ_CONFIG_MAX_PRIORITY];

void luna_obj_init(void)
{
	for (uint32_t i = 0; i < LUNA_OBJ_CONFIG_MAX_PRIORITY; i++) {
		luna_dlist_init(&luna_obj_list[i]);
	}
}


void luna_obj_add(struct core_obj *obj, uint8_t *buffer, uint32_t size, uint32_t priority)
{
	LUNA_ASSERT(obj);
	LUNA_ASSERT(priority < LUNA_OBJ_CONFIG_MAX_PRIORITY);

	obj->priority = priority;

	luna_rq_init(&obj->mailbox, buffer, size, sizeof(void*));
	luna_fsm_init(&obj->super);

	luna_dlist_add_tail(&luna_obj_list[priority], &obj->node);
}

void luna_obj_ev_post(struct core_obj *obj, struct core_ev *ev)
{
	LUNA_ASSERT(obj);
	LUNA_ASSERT(ev);

	luna_ev_ref(ev);
	luna_rq_push(&obj->mailbox, (uint8_t *)&ev);
	luna_obj_ready_map |= 1U << obj->priority;
}

static void luna_obj_run(struct core_obj *obj)
{
	struct core_ev *e;
	if (luna_rq_pop(&obj->mailbox, (uint8_t*)&e)) {
		luna_fsm_dispatch(&obj->super, e);
		luna_ev_gc(e);
	}
}

static struct core_obj *luna_obj_get_highest_ready(void)
{
	if (luna_obj_ready_map == 0) {
		return NULL;
	}

	uint32_t prio = 31 - __builtin_clz(luna_obj_ready_map);
	struct d_list *list = &luna_obj_list[prio];

	struct d_node *n;
	luna_dlist_foreach(list, n) {
		struct core_obj *obj = container_of(n, struct core_obj, node);
		if (!luna_rq_is_empty(&obj->mailbox)) {
			return obj;
		}
	}
	luna_obj_ready_map &= ~(1U << prio);
	return NULL;
}

void luna_obj_schedule(void)
{
	struct core_obj *obj = luna_obj_get_highest_ready();
	if (!obj) {
		return;
	}
	uint32_t prio = obj->priority;
	luna_obj_run(obj);
	luna_dlist_delete(&luna_obj_list[prio], &obj->node);
	luna_dlist_add_tail(&luna_obj_list[prio], &obj->node);
}

#endif
