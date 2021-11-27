#include "c_client_mode_shared.h"
#include "c_net_channel_.h"
#include "../hacks/c_miscellaneous.h"
#include "../hacks/c_legitbot.h"

void c_client_mode_shared::hook()
{
	static c_hook<uintptr_t> hook(client_mode());
	_override_view = hook.apply<override_view_t>(18, override_view);
	_override_mouse_input = hook.apply<override_mouse_input_t>(23, override_mouse_input);
}

uintptr_t* c_client_mode_shared::client_mode()
{
	static auto client_mode = **reinterpret_cast<uintptr_t***>((*reinterpret_cast<uintptr_t**>(base_client()))[10] + 5);
	return client_mode;
}

void c_client_mode_shared::override_view(uintptr_t* client_mode, uint32_t,  c_view_setup* view_setup)
{
	if (!engine_client()->is_ingame())
		return _override_view(client_mode, view_setup);

	c_miscellaneous::set_camera_to_thirdperson();
	_override_view(client_mode, view_setup);
	c_miscellaneous::disable_post_processing();
}

void c_client_mode_shared::override_mouse_input(uintptr_t* client_mode, uint32_t, float* x, float* y)
{
	if (engine_client()->is_ingame())
		c_legitbot::aim(x, y);

	_override_mouse_input(client_mode, x, y);
}
