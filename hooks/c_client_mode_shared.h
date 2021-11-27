#pragma once

#include "../includes.h"
#include "../sdk/misc.h"

class c_client_mode_shared
{
	typedef void(__thiscall* override_view_t)(uintptr_t*, c_view_setup*);
	typedef void(__thiscall* override_mouse_input_t)(uintptr_t*, float*, float*);

public:
	static void hook();

	static uintptr_t* client_mode();
private:
	inline static override_view_t _override_view;
	inline static override_mouse_input_t _override_mouse_input;
	
	static void __fastcall override_view(uintptr_t* client_mode, uint32_t, c_view_setup* view_setup);
	static void __fastcall override_mouse_input(uintptr_t* client_mode, uint32_t, float* x, float* y);
};
