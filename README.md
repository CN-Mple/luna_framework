# luna_framework
``` c
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

#define LUNA_EV_BUS_IMPLEMENTATION
#include "luna_ev_bus.h"
#undef LUNA_EV_BUS_IMPLEMENTATION

#include <stdio.h>
#include <windows.h>

enum {
	SIG_SWITCH_PRESSED = CUSTOM_SIG,
	SIG_LED_ON,
	SIG_LED_OFF,
};

struct led {
	struct core_obj obj;
	int id;
};

struct button {
	struct core_obj obj;
	int count;
};

struct core_ev_bus bus;

struct led led1;
struct led led2;
struct button button;


static int led_state(struct core_fsm *me, const struct core_ev *e)
{
	struct led *led = (struct led *)me;
	switch(e->sig) {
		case SIG_SWITCH_PRESSED: {
			printf("[LED%d] recv.\n", led->id);
			return HAND();
		}
		break;
		default:
			return IGNO();
		break;
	}
	return IGNO();
}

static int switch_state(struct core_fsm *me, const struct core_ev *e)
{
	struct button *button = (struct button *)me;
	switch(e->sig) {
		case SIG_INIT: {
		}
		break;
		case SIG_SWITCH_PRESSED: {
			printf("[switch] pressed count = %d -> publish\n", ++button->count);
			struct core_ev *ev_pressed = luna_ev_new(sizeof(struct core_ev), SIG_SWITCH_PRESSED);
			luna_ps_publish(&bus.ps, ev_pressed);
		}
		break;
		default:
			return IGNO();
		break;
	}
	return IGNO();
}

void obj_run(struct core_obj *obj)
{
	struct core_ev *e;
	if (luna_rq_pop(&obj->mailbox, (uint8_t *)&e)) {
		luna_fsm_dispatch(&obj->super, e);
		luna_ev_gc(e);
	}
}

int key_A_pressed(void)
{
	static int last_state = 0;
	int now_state = (GetAsyncKeyState('A') & 0x8000) ? 1 : 0;
	if (now_state && !last_state) {
		last_state = now_state;
		return 1;
	}
	last_state = now_state;
	return 0;
}

int main(void)
{
	SetConsoleOutputCP(65001);

	luna_ev_bus_init(&bus);
	struct core_ps *ps = &bus.ps;
	struct core_ev ev_switch_pressed;
	ev_switch_pressed.sig = SIG_SWITCH_PRESSED;
	luna_ev_bus_ev_register(&bus, ev_switch_pressed);

	led1.obj.super.handler = led_state;
	static uint8_t buf1[256];
	luna_obj_init(&led1.obj, buf1, sizeof(buf1));
	led1.id = 1;
	luna_ps_subscribe(ps, &ev_switch_pressed, &led1.obj);

	led2.obj.super.handler = led_state;
	static uint8_t buf2[256];
	luna_obj_init(&led2.obj, buf2, sizeof(buf2));
	led2.id = 2;
	luna_ps_subscribe(ps, &ev_switch_pressed, &led2.obj);

	button.obj.super.handler = switch_state;
	static uint8_t buf_switch[256];
	luna_obj_init(&button.obj, buf_switch, sizeof(buf_switch));

	while (1) {
		if (key_A_pressed()) {
			struct core_ev *trigger_ev = luna_ev_new(sizeof(struct core_ev), SIG_SWITCH_PRESSED);
			luna_obj_ev_post(&button.obj, trigger_ev);
		}

		obj_run(&button.obj);
		obj_run(&led1.obj);
		obj_run(&led2.obj);
	}
}

```
