#pragma once

#include "macros.h"

struct beam_info
{
	int32_t	type;

	c_base_entity* start_entity;
	int32_t	start_attachment;
	c_base_entity* end_entity;
	int32_t	end_attachment;

	c_vector3d start;
	c_vector3d end;

	int32_t	model_index;
	const char* model_name;

	int32_t	halo_index;
	const char* halo_name;
	float halo_scale;

	float life;
	float width;
	float end_width;
	float fade_length;
	float amplitude;

	float brightness;
	float speed;

	int32_t	start_frame;
	float frame_rate;

	float red;
	float green;
	float blue;

	bool renderable;
	int32_t	segments;
	int32_t	flags;

	c_vector3d center;
	float start_radius;
	float end_radius;

	beam_info ()
	{
		type = 0;
		segments = -1;
		model_name = nullptr;
		halo_name = nullptr;
		model_index = -1;
		halo_index = -1;
		renderable = true;
		flags = 0;
	}
};

class c_view_render_beams
{
public:
	inline static c_view_render_beams* get()
	{
		static auto view_render_beams = *reinterpret_cast<c_view_render_beams**>(
			sig("client_panorama.dll", "8D 43 FC B9 ? ? ? ? 50 A1") + 0x4);

		return view_render_beams;
	}

	vfunc(4, draw_beam(void* beam), void(__thiscall*)(c_view_render_beams*, void*))(beam)
	vfunc(12, create_beam_points(beam_info& info), void*(__thiscall*)(c_view_render_beams*, beam_info&))(info)
};

#define view_render_beams c_view_render_beams::get()
