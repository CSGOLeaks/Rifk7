#pragma once

#include "c_animation_system.h"
#include "../sdk/c_game_event_manager.h"

namespace resolver
{
	struct shot
	{
		c_animation_system::animation record{};
		c_vector3d start{}, end{};
		uint32_t hitgroup{};
		c_cs_player::hitbox hitbox{};
		float time{}, damage{};
		bool confirmed{}, impacted{}, skip{}, manual{};

		struct server_info_t
		{
			std::vector<c_vector3d> impacts{};
			uint32_t hitgroup{}, damage{}, index{};
		} server_info;
	};
}

class c_resolver
{
public:
	static void resolve(c_animation_system::animation* anim);
	static void register_shot(resolver::shot&& s);
	static void on_player_hurt(c_game_event* event);
	static void on_bullet_impact(c_game_event* event);
	static void on_weapon_fire(c_game_event* event);
	static void on_render_start();

private:
	static void resolve_shot(resolver::shot& shot);

	inline static std::deque<resolver::shot> shots = {};
};
