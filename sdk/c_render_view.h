#pragma once

#include "macros.h"
#include "c_color.h"

class c_render_view
{
	vfunc(6, set_color_modulation_internal(float const* col), void(__thiscall*)(c_render_view*, float const*))(col)

public:
	vfunc(4, set_blend(const float blend), void(__thiscall*)(c_render_view*, float))(blend)

	void set_color_modulation(const c_color color)
	{
		float col[3] = { color.red / 255.f, color.green / 255.f, color.blue / 255.f };
		set_color_modulation_internal(col);
	}
};

interface_var(c_render_view, render_view, "engine.dll", "VEngineRenderView")
