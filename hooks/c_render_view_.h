#pragma once

#include "../sdk/c_render_view.h"

class c_render_view_
{
	typedef void(__thiscall* scene_end_t)(c_render_view*);

public:
	static void hook();

private:
	inline static scene_end_t _scene_end;

	static void __fastcall scene_end(c_render_view* render_view, uint32_t);
};
