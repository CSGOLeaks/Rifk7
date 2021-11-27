#pragma once

#include "../sdk/recv.h"

class c_smoke_grenade_projectile_
{
public:
	static void hook();

private:
	// ReSharper disable CppInconsistentNaming
	inline static recv_var_proxy_fn _m_bDidSmokeEffect;

	static void __cdecl m_bDidSmokeEffect(c_recv_proxy_data* data, void* entity, void* output);
	// ReSharper restore CppInconsistentNaming
};