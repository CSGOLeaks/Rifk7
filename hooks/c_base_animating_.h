#pragma once

#include "../sdk/recv.h"

class c_base_animating_
{
public:
	static void hook();

private:
	// ReSharper disable CppInconsistentNaming
	inline static recv_var_proxy_fn _m_bClientSideAnimation;

	static void __cdecl m_bClientSideAnimation(c_recv_proxy_data* data, void* entity, void* output);
	// ReSharper restore CppInconsistentNaming
};
