#pragma once

#include <cstdint>

class convar;

class c_sv_cheats
{
	typedef bool(__thiscall* get_bool_t)(convar*);

public:
	static void hook();

private:
	inline static get_bool_t _get_bool;

	static bool __fastcall get_bool(convar* var, uint32_t);
};
