#include "c_sv_cheats.h"
#include "../sdk/c_cvar.h"
#include "../utils/c_hook.h"
#include <intrin.h>

void c_sv_cheats::hook()
{
	static c_hook<convar> hook(cvar()->find_var(_("sv_cheats")));
	_get_bool = hook.apply<get_bool_t>(13, get_bool);
}

bool __fastcall c_sv_cheats::get_bool(convar* var, uint32_t)
{
	static const auto return_to_cam_think = sig("client_panorama.dll", "85 C0 75 30 38 86");

	if (_ReturnAddress() == return_to_cam_think)
		return true;		

	return _get_bool(var);
}
