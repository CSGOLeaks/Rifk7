#pragma once

#include "../includes.h"

class c_wnd_proc
{
	static auto constexpr previous_state_mask = 0x40000000;

public:
    static void hook();

	inline static HWND window;

private:
	static LRESULT __stdcall wnd_proc(HWND wnd, UINT msg, WPARAM w_param, LPARAM l_param);

	inline static WNDPROC _wnd_proc;
};
