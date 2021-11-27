#include "c_cl_csm_shadows.h"
#include "../sdk/c_cvar.h"
#include "../utils/c_hook.h"

void c_cl_csm_shadows::hook()
{
	static c_hook<convar> hook(cvar()->find_var(_("cl_csm_shadows")));
	hook.apply<void(__thiscall*)()>(13, get_bool);
}

bool __fastcall c_cl_csm_shadows::get_bool(convar*, uint32_t)
{
	return false;
}
