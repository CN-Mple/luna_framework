# luna_framework
``` c
/* main.c */
#define LUNA_FRAMEWORK_IMPLEMENTATION
#include "luna_framework.h"

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

#define LED_MAX_COUNT		(5)
#define LED_BUFFER_SIZE		(256)

#define BUTTON_BUFFER_SIZE	(256)

struct led led[LED_MAX_COUNT];
static uint8_t led_buffer[LED_MAX_COUNT][LED_BUFFER_SIZE];

struct button button;
static uint8_t button_buffer[BUTTON_BUFFER_SIZE];

static int switch_state(struct core_fsm *me, const struct core_ev *e)
{
	struct button *btn = (struct button *)me;
	switch(e->sig) {
        case SIG_INIT: {

        }
            return HAND();
        case SIG_ENTER: {

        }
            return HAND();
        case SIG_EXIT: {

        }
            return HAND();
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

static int led_state(struct core_fsm *me, const struct core_ev *e)
{
	struct led *led = (struct led *)me;
	switch(e->sig) {
        case SIG_INIT: {

        }
            return HAND();
        case SIG_ENTER: {
            struct core_ev ev_switch = { .sig = SIG_SWITCH_PRESSED };
            luna_ps_subscribe(luna_framework_get_ps(), &ev_switch, &led->obj);
        }
            return HAND();
        case SIG_EXIT: {
            struct core_ev ev_switch = { .sig = SIG_SWITCH_PRESSED };
            luna_ps_unsubscribe(luna_framework_get_ps(), &ev_switch, &led->obj);
        }
            return HAND();
		case SIG_SWITCH_PRESSED: {
			printf("[LED%d] recv.\n", led->id);
        }
            return HAND();
		default:
            return IGNO();
	}
}

static int key_A_pressed(void)
{
	static int last = 0;
	int now = (GetAsyncKeyState('A') & 0x8000) ? 1 : 0;
	int ret = 0;
	if (now && !last) ret = 1;
	last = now;
	return ret;
}

static void user_init(void)
{
	for (size_t i = 0; i < LED_MAX_COUNT; ++i) {
		led[i].obj.super.handler = led_state;
		luna_obj_add(&led[i].obj, &led_buffer[i][0], LED_BUFFER_SIZE, 1);
		led[i].id = i;
	}

	button.obj.super.handler = switch_state;
	luna_obj_add(&button.obj, button_buffer, BUTTON_BUFFER_SIZE, 2);
}

int main(void)
{
	SetConsoleOutputCP(65001);
	luna_framework_init();

	struct core_ev ev_switch = { .sig = SIG_SWITCH_PRESSED };
	luna_ev_bus_ev_register(&bus, ev_switch);

	user_init();

	while (1) {
		if (key_A_pressed()) {
			struct core_ev *ev = luna_ev_new(sizeof(struct core_ev), SIG_SWITCH_PRESSED);
			luna_obj_ev_post(&button.obj, ev);
		}
		luna_framework_run();
	}
}

```
