# luna_framework
``` c
/* main.c */
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
		case SIG_SWITCH_PRESSED:
			printf("[LED%d] recv.\n", led->id);
			return HAND();
		default:
			return IGNO();
	}
}

static int switch_state(struct core_fsm *me, const struct core_ev *e)
{
	struct button *btn = (struct button *)me;
	switch(e->sig) {
		case SIG_SWITCH_PRESSED: {
			printf("[switch] pressed count = %d -> publish\n", ++btn->count);
			struct core_ev *ev = luna_ev_new(sizeof(struct core_ev), SIG_SWITCH_PRESSED);
			luna_ps_publish(&bus.ps, ev);
			return HAND();
		}
		default:
			return IGNO();
	}
}

int key_A_pressed(void)
{
	static int last = 0;
	int now = (GetAsyncKeyState('A') & 0x8000) ? 1 : 0;
	int ret = 0;
	if (now && !last) ret = 1;
	last = now;
	return ret;
}

int main(void)
{
	SetConsoleOutputCP(65001);

    luna_obj_init();
	luna_ev_bus_init(&bus);

	struct core_ev ev_switch = { .sig = SIG_SWITCH_PRESSED };
	luna_ev_bus_ev_register(&bus, ev_switch);

	// LED1
	led1.obj.super.handler = led_state;
	static uint8_t buf1[256];
	luna_obj_add(&led1.obj, buf1, sizeof(buf1), 1);
	led1.id = 1;
	luna_ps_subscribe(&bus.ps, &ev_switch, &led1.obj);

	// LED2
	led2.obj.super.handler = led_state;
	static uint8_t buf2[256];
	luna_obj_add(&led2.obj, buf2, sizeof(buf2), 1);
	led2.id = 2;
	luna_ps_subscribe(&bus.ps, &ev_switch, &led2.obj);

	// Button
	button.obj.super.handler = switch_state;
	static uint8_t buf_btn[256];
	luna_obj_add(&button.obj, buf_btn, sizeof(buf_btn), 2);

	while (1) {
		if (key_A_pressed()) {
			struct core_ev *ev = luna_ev_new(sizeof(struct core_ev), SIG_SWITCH_PRESSED);
			luna_obj_ev_post(&button.obj, ev);
		}
		luna_obj_schedule();
	}
}

```
