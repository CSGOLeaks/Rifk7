#pragma once

#include <cstdint>

class convar;

class c_cl_csm_shadows
{
public:
    static void hook();

private:
	static bool __fastcall get_bool(convar*, uint32_t);
};
