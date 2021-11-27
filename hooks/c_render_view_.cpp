#include "c_render_view_.h"
#include "../utils/c_hook.h"
#include "../hacks/c_chams.h"

void c_render_view_::hook()
{
	static c_hook<c_render_view> hook(render_view());
	_scene_end = hook.apply<scene_end_t>(9, scene_end);
}

void __fastcall c_render_view_::scene_end(c_render_view* render_view, uint32_t)
{
	_scene_end(render_view);

	if (engine_client()->is_ingame())
		c_chams::draw_players();
}
