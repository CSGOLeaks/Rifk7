#pragma once

#include <cstdint>

class c_mdl_cache
{
	typedef bool(__thiscall* unknown_fn_t)(void*);

public:
	static void hook();

private:
	inline static unknown_fn_t _unknown_fn;

	static bool __fastcall unknown_fn(void* mdl_cache, uint32_t);
};
