#pragma once
#include <cstdint>
#include "../sdk/misc.h"

class c_view_render;

class c_view_render_
{
	typedef void(__thiscall* render_view_t)(c_view_render*, c_view_setup&, c_view_setup&, uint32_t, int32_t);

public:
	static void hook();

private:
	inline static render_view_t _render_view;

	static void __fastcall render_view(c_view_render* render, uint32_t, c_view_setup& setup, c_view_setup& hud_setup, uint32_t clear_flags, int32_t draw);
};
