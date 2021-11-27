#pragma once

#include "c_base_combat_character.h"

class c_base_player : public c_base_combat_character
{
public:
	enum flags
	{
		on_ground = 1 << 0,
		ducking = 1 << 1,
		water_jump = 1 << 2,
		on_train = 1 << 3,
		in_rain = 1 << 4,
		frozen = 1 << 5,
		at_controls = 1 << 6,
		client = 1 << 7,
		fake_client = 1 << 8,
		in_water = 1 << 9,
		fly = 1 << 10,
		swim = 1 << 11,
		conveyor = 1 << 12,
		npc = 1 << 13,
		god_mode = 1 << 14,
		no_target = 1 << 15,
		aim_target = 1 << 16,
		partial_ground = 1 << 17,
		static_prop = 1 << 18,
		graphed = 1 << 19,
		grenade = 1 << 20,
		step_movement = 1 << 21,
		dont_touch = 1 << 22,
		base_velocity = 1 << 23,
		world_brush = 1 << 24,
		object = 1 << 25,
		kill_me = 1 << 26,
		on_fire = 1 << 27,
		dissolving = 1 << 28,
		tansragdoll = 1 << 29,
		unblockable_by_player = 1 << 30
	};

	bool is_on_ground()
	{
		return get_flags() & on_ground || get_flags() & partial_ground || get_flags() & conveyor;
	}

	bool is_grounded(std::optional<int32_t> flags = std::nullopt)
	{
		return flags.value_or(get_flags()) & on_ground;
	}

	netvar(get_flags(), int32_t, "CBasePlayer", "m_fFlags")
	netvar_offset(get_third_person_angles(), c_qangle, "CBasePlayer", "deadflag", 4)
	netvar(get_tick_base(), int32_t, "CBasePlayer", "m_nTickBase")
	netvar(get_view_model(), c_base_handle, "CBasePlayer", "m_hViewModel[0]")
	netvar(get_velocity(), c_vector3d, "CBasePlayer", "m_vecVelocity[0]")

	datamap(get_ground_entity(), c_base_handle, "m_hGroundEntity")
};
