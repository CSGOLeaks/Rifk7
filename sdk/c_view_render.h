#pragma once
#include "../utils/c_memory.h"

class c_view_render
{
public:
	static c_view_render* get()
	{
		static const auto view_render = **reinterpret_cast<c_view_render***>(reinterpret_cast<uint32_t>(
			sig("client_panorama.dll", "8B 0D ? ? ? ? 57 8B 01 FF 50 14 E8 ? ? ? ? 5F")) + 2);
		return view_render;
	}
};

#define view_render c_view_render::get()
