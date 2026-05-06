/* luna_fsm.h */
#ifndef LUNA_FSM_H
#define LUNA_FSM_H

#include "luna_ev.h"

#define HANDLED		(uint32_t)(0)
#define IGNORED		(uint32_t)(1)
#define TRANSFER	(uint32_t)(2)

#define HAND()		HANDLED
#define IGNO()		IGNORED
#define TRAN(t) \
			me->handler = t, TRANSFER

struct core_fsm;

typedef int (*Handler)(struct core_fsm *fsm, struct core_ev const*e);

struct core_fsm {
	Handler handler;
};

enum {
	SIG_EMPTY = 0,
	SIG_ENTER,
	SIG_EXIT,
	SIG_INIT,
	SIG_INTERRUPT,
	SIG_RESERVE_SIG_NUM,
	CUSTOM_SIG = SIG_RESERVE_SIG_NUM,
};

void luna_fsm_init(struct core_fsm *me);
void luna_fsm_dispatch(struct core_fsm *me, struct core_ev *e);

#endif

#ifdef LUNA_FSM_IMPLEMENTATION

const struct core_ev luna_interal_sig[] = {
	{.sig = SIG_EMPTY},
	{.sig = SIG_ENTER},
	{.sig = SIG_EXIT},
	{.sig = SIG_INIT},
	{.sig = SIG_INTERRUPT},
};

void luna_fsm_init(struct core_fsm *me)
{
	LUNA_ASSERT(me);
	LUNA_ASSERT(me->handler);

	(void)(*me->handler)(me, &luna_interal_sig[SIG_INIT]);
	(void)(*me->handler)(me, &luna_interal_sig[SIG_ENTER]);
}

void luna_fsm_dispatch(struct core_fsm *me, struct core_ev *e)
{
	LUNA_ASSERT(me);
	LUNA_ASSERT(e);

	Handler handler = me->handler;
	LUNA_ASSERT(handler != NULL);

	uint32_t result = handler(me, e);
	if (TRANSFER == result) {
		(void)handler(me, &luna_interal_sig[SIG_EXIT]);
		(void)(*me->handler)(me, &luna_interal_sig[SIG_ENTER]);
	}
}

#endif

