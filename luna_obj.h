/* luna_obj.h */
#ifndef LUNA_OBJ_H
#define LUNA_OBJ_H

#include "luna_rq.h"
#include "luna_ev.h"
#include "luna_fsm.h"

struct core_obj {
	struct core_fsm super;
	struct rq mailbox;
};

void luna_obj_init(struct core_obj *obj, uint8_t *buffer, uint32_t size);
void luna_obj_ev_post(struct core_obj *obj, struct core_ev *ev);

#endif

#ifdef LUNA_OBJ_IMPLEMENTATION

void luna_obj_init(struct core_obj *obj, uint8_t *buffer, uint32_t size)
{
	LUNA_ASSERT(obj);
	luna_rq_init(&obj->mailbox, buffer, size, sizeof(void*));
	luna_fsm_init(&obj->super);
}

void luna_obj_ev_post(struct core_obj *obj, struct core_ev *ev)
{
	LUNA_ASSERT(obj);
	LUNA_ASSERT(ev);
	luna_ev_ref(ev);
	luna_rq_push(&obj->mailbox, (uint8_t *)&ev);
}

#endif
