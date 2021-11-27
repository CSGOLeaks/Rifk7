#pragma once

#include "macros.h"

class c_base_view_model : public c_base_entity
{
public:
	netvar(get_weapon(), c_base_handle, "CBaseViewModel", "m_hWeapon")
	datamap(get_cycle(), float, "m_flCycle")
	datamap(get_anim_time(), float, "m_flAnimTime")
	datamap(get_sequence(), int, "m_nSequence")
	vfunc(241, send_matching_seq(int seq), void(__thiscall*)(c_base_view_model*, int))(seq)
};
