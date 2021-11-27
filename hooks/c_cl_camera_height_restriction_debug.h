#pragma once

class convar;

class c_cl_camera_height_restriction_debug
{
public:
    static void hook();

	static int __stdcall proxy_get_bool(convar* var, void* return_address, void* eax);

	inline static bool in_cm = false;
};

static bool get_bool(void* eax, void* edx);
