/* luna_ev.h */
#ifndef LUNA_EV_H
#define LUNA_EV_H

#include "luna_rc.h"
typedef uint32_t core_sig_t;

struct core_ev {
	core_sig_t sig;
};

struct core_ev *luna_ev_new(size_t size, core_sig_t sig);
void luna_ev_ref(struct core_ev *ev);
void luna_ev_gc(struct core_ev *ev);

#endif

#ifdef LUNA_EV_IMPLEMENTATION

static void luna_ev_destroy(void *data)
{
	(void)data;
}

struct core_ev *luna_ev_new(size_t size, core_sig_t sig)
{
	struct core_ev *ev = luna_rc_alloc(size, luna_ev_destroy);
	ev->sig = sig;
	return ev;
}

void luna_ev_ref(struct core_ev *ev)
{
	LUNA_ASSERT(ev);
	luna_rc_acquire(ev);
}

void luna_ev_gc(struct core_ev *ev)
{
	LUNA_ASSERT(ev);
	luna_rc_release(ev);
}

#endif

