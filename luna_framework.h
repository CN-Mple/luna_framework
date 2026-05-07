/* luna_framework.h */
#ifndef LUNA_FRAMEWORK_H
#define LUNA_FRAMEWORK_H

#include "luna_slist.h"
#include "luna_dlist.h"

#include "luna_rc.h"
#include "luna_ev.h"
#include "luna_fsm.h"
#include "luna_rq.h"
#include "luna_obj.h"
#include "luna_ps.h"
#include "luna_ev_bus.h"

struct core_ps *luna_framework_get_ps(void);

void luna_framework_init(void);
void luna_framework_run(void);

#endif

#ifdef LUNA_FRAMEWORK_IMPLEMENTATION

#define LUNA_RC_IMPLEMENTATION
#include "luna_rc.h"
#undef LUNA_RC_IMPLEMENTATION

#define LUNA_EV_IMPLEMENTATION
#include "luna_ev.h"
#undef LUNA_EV_IMPLEMENTATION

#define LUNA_FSM_IMPLEMENTATION
#include "luna_fsm.h"
#undef LUNA_FSM_IMPLEMENTATION

#define LUNA_RQ_IMPLEMENTATION
#include "luna_rq.h"
#undef LUNA_RQ_IMPLEMENTATION

#define LUNA_OBJ_IMPLEMENTATION
#include "luna_obj.h"
#undef LUNA_OBJ_IMPLEMENTATION

#define LUNA_PS_IMPLEMENTATION
#include "luna_ps.h"
#undef LUNA_PS_IMPLEMENTATION

#define LUNA_SLIST_IMPLEMENTATION
#include "luna_slist.h"
#undef LUNA_SLIST_IMPLEMENTATION

#define LUNA_DLIST_IMPLEMENTATION
#include "luna_dlist.h"
#undef LUNA_DLIST_IMPLEMENTATION

#define LUNA_EV_BUS_IMPLEMENTATION
#include "luna_ev_bus.h"
#undef LUNA_EV_BUS_IMPLEMENTATION

static struct core_ev_bus bus;

struct core_ps *luna_framework_get_ps(void)
{
	return &bus.ps;
}

void luna_framework_init(void)
{
	luna_obj_init();
	luna_ev_bus_init(&bus);
}

void luna_framework_run(void)
{
	luna_obj_schedule();
}

#endif
