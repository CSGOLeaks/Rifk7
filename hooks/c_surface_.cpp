#include "c_surface_.h"
#include "../menu/c_menu.h"

void c_surface_::hook()
{
	static c_hook<c_surface> hook(surface());
	_lock_cursor = hook.apply<lock_cursor_t>(67, lock_cursor);
}

void c_surface_::lock_cursor(c_surface* surface, uint32_t)
{
	if (menu->is_open())
		surface->unlock_cursor();
	else
		_lock_cursor(surface);
}
