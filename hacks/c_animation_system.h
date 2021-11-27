#pragma once

#include <deque>
#include <utility>
#include <mutex>
#include "../sdk/c_cs_player.h"
#include "../sdk/c_client_state.h"
#include "../sdk/c_global_vars_base.h"
#include "../sdk/c_cvar.h"

enum resolver_state {
	resolver_start,
	resolver_inverse,
	resolver_no_desync,
	resolver_jitter
};

__forceinline float calculate_lerp()
{
	static auto cl_interp = cvar()->find_var(_("cl_interp"));
	static auto cl_updaterate = cvar()->find_var(_("cl_updaterate"));

	const auto update_rate = cl_updaterate->get_int();
	const auto interp_ratio = cl_interp->get_float();

	auto lerp = interp_ratio / update_rate;

	if (lerp <= interp_ratio)
		lerp = interp_ratio;

	return lerp;
}

class c_animation_system : public c_singleton<c_animation_system>
{
public:
	struct animation
	{
		animation() = default;
		explicit animation(c_cs_player* player);
		explicit animation(c_cs_player* player, c_qangle last_reliable_angle);
		void restore(c_cs_player* player) const;
		void apply(c_cs_player* player) const;
		void build_server_bones(c_cs_player* player);
		__forceinline bool is_valid(const float range = .2f, const float max_unlag = .2f) const
		{
			if (!net_channel || !valid)
				return false;

			const auto correct = std::clamp(net_channel->get_latency(flow_incoming)
				+ net_channel->get_latency(flow_outgoing)
				+ calculate_lerp(), 0.f, max_unlag);

			return fabsf(correct - (global_vars_base->curtime - sim_time)) <= range;
		}

		c_cs_player* player{};
		int32_t index{};

		bool valid{}, has_anim_state{};
		alignas(16) matrix3x4 bones[128]{};

		bool dormant{};
		c_vector3d velocity;
		c_vector3d origin;
		c_vector3d abs_origin;
		c_vector3d obb_mins;
		c_vector3d obb_maxs;
		c_base_animating::animation_layers layers{};
		c_base_animating::pose_paramater poses{};
		c_csgo_player_anim_state anim_state{};
		float anim_time{};
		float sim_time{};
		float interp_time{};
		float duck{};
		float lby{};
		float last_shot_time{};
		c_qangle last_reliable_angle{};
		c_qangle eye_angles;
		c_qangle abs_ang;
		int flags{};
		int eflags{};
		int effects{};
		int lag{};
		int shot{};
	};
private:
	struct animation_info {
		animation_info(c_cs_player* player, std::deque<animation> animations)
			: player(player), frames(std::move(animations)), last_spawn_time(0) { }

		void update_animations(animation* to, animation* from);

		c_cs_player* player{};
		std::deque<animation> frames{};

		// latest animation (might be invalid)
		animation latest_animation{};

		// last time this player spawned
		float last_spawn_time;

		// counter of how many shots we missed
		int32_t missed_due_to_spread{};
		
		// resolver data
		resolver_state brute_state{};
		float brute_yaw{};
		c_vector3d last_reliable_angle{};
	};

	std::unordered_map<c_base_handle, animation_info> animation_infos;

public:
	void update_player(c_cs_player* player);
	void update_simple_local_player(c_cs_player* player, c_user_cmd* cmd);
	void post_player_update();

	animation_info* get_animation_info(c_cs_player* player);
	std::optional<animation*> get_latest_animation(c_cs_player* player);
	std::optional<animation*> get_oldest_animation(c_cs_player* player);
	std::optional<animation*> get_uncrouched_animation(c_cs_player* player);
	std::optional<std::pair<animation*, animation*>> get_intermediate_animations(c_cs_player* player, float range = 1.f);
	std::vector<animation*> get_valid_animations(c_cs_player* player, float range = 1.f);
	std::optional<animation*> get_lastest_animation_unsafe(c_cs_player* player);

	animation local_animation;
	c_base_animating::animation_layers server_layers{};
	bool in_jump{}, enable_bones{};
	c_csgo_player_anim_state* last_process_state{};
};

#define animation_system c_animation_system::instance()
#define lerp_ticks time_to_ticks(calculate_lerp())
