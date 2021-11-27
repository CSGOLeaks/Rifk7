#include "c_view_render_.h"
#include "../sdk/c_view_render.h"
#include "../sdk/c_engine_client.h"
#include "../utils/c_hook.h"
#include "../hacks/c_miscellaneous.h"

void c_view_render_::hook()
{
	static c_hook<c_view_render> hook(view_render);
	_render_view = hook.apply<render_view_t>(6, render_view);
}

void __fastcall c_view_render_::render_view(c_view_render* render, uint32_t, c_view_setup& setup, c_view_setup& hud_setup,
	const uint32_t clear_flags, const int32_t draw)
{
	if (engine_client()->is_ingame())
	{
		c_miscellaneous::remove_visual_recoil(setup);
		c_miscellaneous::change_fov(setup);
	}
	
	return _render_view(render, setup, hud_setup, clear_flags, draw);
}
