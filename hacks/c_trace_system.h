#pragma once

#include <optional>
#include "../utils/c_singleton.h"
#include "../sdk/c_cs_player.h"
#include "../sdk/c_engine_trace.h"
#include "c_animation_system.h"

class c_trace_system : public c_singleton<c_trace_system>
{
public:
	struct wall_pen
	{
		float damage;
		c_cs_player::hitbox hitbox;
		int32_t hitgroup;
	};

	std::optional<wall_pen> wall_penetration(c_vector3d src, c_vector3d end,
		c_animation_system::animation* target, c_cs_player* override_player = nullptr) const;

	static void run_emulated(c_animation_system::animation* target, std::function<void()> fn);

	static void extrapolate(c_cs_player* player, c_vector3d& origin, c_vector3d& velocity, int& flags, bool on_ground);

private:
	static std::optional<wall_pen> fire_bullet(c_base_combat_weapon::weapon_data* data, c_vector3d src,
		c_vector3d pos, trace_filter* filter, c_cs_player* target = nullptr, bool point = false);

	static bool handle_bullet_penetration(c_base_combat_weapon::weapon_data* weapon_data, game_trace& enter_trace,
                                   c_vector3d& eye_position, c_vector3d direction, int& penetrate_count,
                                   float& current_damage, float penetration_power);
	static bool trace_to_exit(game_trace& enter_trace, game_trace& exit_trace, c_vector3d start_position, c_vector3d direction, bool is_local = false);

	static float scale_damage(c_cs_player* target, float damage, float weapon_armor_ratio, int hitgroup, bool is_zeus);
};

#define trace_system c_trace_system::instance()
