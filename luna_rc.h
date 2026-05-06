/* luna_rc.h */
#ifndef LUNA_RC_H
#define LUNA_RC_H

#include <stdint.h>

#ifndef LUNA_ASSERT
#include <assert.h>
#define LUNA_ASSERT	assert
#endif

#ifndef LUNA_MALLOC
#include <stdlib.h>
#define LUNA_MALLOC	malloc
#endif

#ifndef LUNA_FREE
#include <stdlib.h>
#define LUNA_FREE	free
#endif

struct rc {
	uint32_t count;
	void (*destroy)(void *data);
};

void *luna_rc_alloc(size_t size, void (*destroy)(void*));
void *luna_rc_acquire(void *data);
void luna_rc_release(void *data);

#endif

#ifdef LUNA_RC_IMPLEMENTATION

void *luna_rc_alloc(size_t size, void (*destroy)(void*))
{
	LUNA_ASSERT(size > 0);
	struct rc *_rc = LUNA_MALLOC(sizeof(struct rc) + size);
	LUNA_ASSERT(_rc);
	_rc->count = 0;
	_rc->destroy = destroy;
	return (_rc + 1);
}

void *luna_rc_acquire(void *data)
{
	LUNA_ASSERT(data);
	struct rc *_rc = (struct rc *)data - 1;
	_rc->count += 1;
	return (_rc + 1);
}

void luna_rc_release(void *data)
{
	LUNA_ASSERT(data);
	struct rc *_rc = (struct rc *)data - 1;
	if (_rc->count > 0) {
		if (--_rc->count == 0) {
			if (_rc->destroy) {
				_rc->destroy(data);
			}
			LUNA_FREE(_rc);
		}
	}
}

#endif
