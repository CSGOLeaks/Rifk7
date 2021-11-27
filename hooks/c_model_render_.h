#pragma once

#include "../sdk/c_model_render.h"
#include "../sdk/c_model_info_client.h"

class c_model_render_
{
	typedef void(__thiscall* draw_model_execute_t)(c_model_render*, void*, draw_model_state*, model_render_info_t&, matrix3x4*);

public:
	static void hook();

private:
	static auto constexpr studiorender_draw_translucent_only = 2;

	inline static draw_model_execute_t _draw_model_execute;

	static void __fastcall draw_model_execute(c_model_render* model_render, uint32_t, void* ctx,
		draw_model_state* state, model_render_info_t& info, matrix3x4* matrix);
};
