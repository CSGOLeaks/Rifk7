#include "c_antiaim.h"
#include "../utils/math.h"
#include "../sdk/c_game_rules.h"
#include "../sdk/c_client_state.h"
#include "../sdk/c_debug_overlay.h"
#include "../sdk/c_input.h"
#include "../sdk/c_prediction.h"
#include "../sdk/c_weapon_system.h"
#include "../menu/c_menu.h"
#include "c_aimhelper.h"
#include "c_miscellaneous.h"
#include "c_ragebot.h"
#include "c_trace_system.h"
#include "c_prediction_system.h"
#include "../hooks/c_events.h"
#include <random>

static std::random_device rd;
static std::mt19937 rng(rd());

void c_antiaim::fakelag(c_cs_player* local, c_user_cmd* cmd, bool& send_packet)
{
	static constexpr auto target_standing = 1;
	static constexpr auto target_moving = 13;
	static constexpr auto target_air = 8;

	static auto last_origin = c_vector3d();
	static auto last_simtime = 0.f;
	static auto last_ducked = false;
	static auto fakeland = false;

	static auto onpeek_timer = 0.f;
	static auto unduck_timer = 0.f;

	static auto onpeek = false;

	if (!local || !local->is_alive() || !cmd || game_rules->is_freeze_period())
		return;

	const auto weapon = reinterpret_cast<c_base_combat_weapon*>(
		client_entity_list()->get_client_entity_from_handle(local->get_current_weapon_handle()));

	if (!weapon)
		return;

	const auto info = weapon_system->get_weapon_data(weapon->get_item_definition());

	if (!info)
		return;

	const auto max_choke_amount = game_rules->is_valve_ds() ? 9u : 15u;
	auto choke_amount = target_standing;

	if (local->get_velocity().length2d() > info->get_standing_accuracy(weapon) || local->get_duck_amount() > 0.f)
		choke_amount = target_moving;

	if (!local->is_on_ground())
		choke_amount = target_air;

	// are we in on peek?
	if (onpeek_timer > global_vars_base->curtime + ticks_to_time(16))
	{
		onpeek_timer = 0.f;
		onpeek = false;
	}

	// are we in unduck?
	if (unduck_timer > global_vars_base->curtime + ticks_to_time(16))
		unduck_timer = 0.f;

	// extend fake lag to maximum.
	if (onpeek_timer >= global_vars_base->curtime || unduck_timer >= global_vars_base->curtime)
		choke_amount = max_choke_amount + 1;

	// force enemy to extrapolate us.
	if (last_simtime == global_vars_base->curtime - global_vars_base->interval_per_tick
		&& (local->get_origin() - last_origin).length2dsqr() > 4096.f)
		choke_amount = 1;

	auto missing_target = false;

	if (on_peek(local, missing_target))
	{
		if (!onpeek && onpeek_timer < global_vars_base->curtime)
		{
			onpeek_timer = global_vars_base->curtime + ticks_to_time(16);
			if (client_state->choked_commands > 1)
				choke_amount = 1;
		}

		onpeek = true;
	}
	else if (missing_target)
		onpeek = false;

	if (local->get_flags() & c_base_player::flags::on_ground
		&& !(cmd->buttons & c_user_cmd::flags::duck) && last_ducked)
	{
		if (unduck_timer < global_vars_base->curtime)
		{
			unduck_timer = global_vars_base->curtime + ticks_to_time(16);

			if (client_state->choked_commands > 1)
			{
				cmd->buttons |= c_user_cmd::flags::duck;
				choke_amount = 1;
			}
		}
	}

	if (unduck_timer > global_vars_base->curtime - ticks_to_time(2) && !client_state->choked_commands)
		cmd->buttons |= c_user_cmd::flags::duck;

	// fake land to desync animations for pasted cheats.
	auto origin = local->get_origin();
	auto velocity = local->get_velocity();
	auto flags = local->get_flags();
	c_trace_system::extrapolate(local, origin, velocity, flags,
		prediction_system->unpredicted_flags & c_base_player::on_ground);
	if (flags & c_base_player::flags::on_ground
		&& !(local->get_flags() & c_base_player::flags::on_ground))
	{
		fakeland = fabsf(math::normalize_yaw(local->get_lby()
			- animation_system->local_animation.eye_angles.y)) > 35.f;

		choke_amount = fakeland ? max_choke_amount : 0;
	}
	else if (local->get_flags() & c_base_player::flags::on_ground && fakeland)
	{
		fakeland = false;
		choke_amount = 1;
	}

	if (global_vars_base->curtime + global_vars_base->interval_per_tick > next_lby_update
		|| lby_update >= global_vars_base->curtime - global_vars_base->interval_per_tick)
		choke_amount = max_choke_amount;

	// set send packet and stats.
	const auto shot_last_tick = antiaim->shot_cmd <= cmd->command_number
		&& antiaim->shot_cmd > cmd->command_number - client_state->choked_commands;
	send_packet = shot_last_tick || client_state->choked_commands >= choke_amount;
	estimated_choke = choke_amount;	

	// store data of current tick for next evaluation.
	last_origin = local->get_origin();
	last_simtime = global_vars_base->curtime;
	last_ducked = cmd->buttons & c_user_cmd::flags::duck;
}

void c_antiaim::run(c_cs_player* local, c_user_cmd* cmd)
{
	static constexpr auto target_delta = -60.f;
	static auto alternate = false;

	static const auto get_check_sum = reinterpret_cast<uint32_t(__thiscall*)(c_user_cmd*)>(
		sig("client_panorama.dll", "53 8B D9 83 C8"));

	const auto weapon = reinterpret_cast<c_base_combat_weapon*>(
		client_entity_list()->get_client_entity_from_handle(local->get_current_weapon_handle()));

	if (!weapon ||
		local->get_move_type() == c_base_player::movetype_observer ||
		local->get_move_type() == c_base_player::movetype_noclip ||
		local->get_move_type() == c_base_player::movetype_ladder ||
		local->get_flags() & c_base_player::frozen ||
		game_rules->is_freeze_period())
	{
		animation_system->update_simple_local_player(local, cmd);
		return;
	}

	const auto ideal_yaw = math::normalize_yaw(calculate_ideal_yaw(local));
	*local->get_animation_layers() = animation_system->server_layers;

	for (auto i = 0; i <= client_state->choked_commands; i++)
	{
		auto& c = input->commands[(cmd->command_number + i - client_state->choked_commands) % 150];
		auto& verified = input->verified_commands[(cmd->command_number + i - client_state->choked_commands) % 150];

		if (c.command_number >= shot_cmd && shot_cmd >= cmd->command_number - client_state->choked_commands)
		{
			auto target_angle = c.viewangles;
			c.viewangles = input->commands[shot_cmd % 150].viewangles;
			c_aimhelper::fix_movement(&c, target_angle);
			math::ensure_bounds(c.viewangles, *reinterpret_cast<c_vector3d*>(&c.forwardmove));
			c_miscellaneous::set_buttons_for_direction(&c);
		}
		else
		{
			auto target_angle = c.viewangles;

			c.viewangles.x = 89.f;
			c.viewangles.y = last_real = ideal_yaw;
			c.viewangles.z = 0.f;

			last_fake = math::normalize_yaw(ideal_yaw - target_delta + max_delta);

			if (i != client_state->choked_commands)
			{
				const auto info = prediction_system->animation_info[
					(cmd->command_number + i - client_state->choked_commands) % 150];				

				// make room for prediction error.
				const auto min = min_delta + .1f;
				const auto max = max_delta - .1f;

				// change break direction, let the enemy guess what direction we are faking.
				const auto delta = alternate ? min : min;
				alternate = !alternate;

				if (lby_update == info.curtime + global_vars_base->interval_per_tick || info.curtime + global_vars_base->interval_per_tick > next_lby_update)
					c.viewangles.y = math::normalize_yaw(ideal_yaw - target_delta + delta);
				else if (lby_update == info.curtime || info.curtime > next_lby_update)
					c.viewangles.y = math::normalize_yaw(ideal_yaw + target_delta);
				else
					c.viewangles.y = math::normalize_yaw(ideal_yaw - target_delta + max);
			}

			c_aimhelper::fix_movement(&c, target_angle);
			math::ensure_bounds(c.viewangles, *reinterpret_cast<c_vector3d*>(&c.forwardmove));
			c_miscellaneous::set_buttons_for_direction(&c);
		}

		verified.cmd = c;
		verified.crc = get_check_sum(&verified.cmd);

		// fix up prediction and animate.
		prediction_system->repredict(local, &c);

		local->get_anim_state()->feet_yaw_rate = 0.f;
		animation_system->local_animation.eye_angles = c.viewangles;
		animation_system->update_player(local);
	}

	animation_system->local_animation.abs_ang.y = local->get_anim_state()->goal_feet_yaw;
	animation_system->local_animation.layers = animation_system->server_layers;
	animation_system->local_animation.poses = local->get_pose_parameter();
}

void c_antiaim::prepare_animation(c_cs_player* local)
{
	const auto state = local->get_anim_state();

	*local->get_animation_layers() = animation_system->local_animation.layers;
	local->get_pose_parameter() = animation_system->local_animation.poses;

	if (local->get_move_type() == c_base_player::movetype_observer ||
		local->get_move_type() == c_base_player::movetype_noclip ||
		local->get_move_type() == c_base_player::movetype_ladder ||
		local->get_flags() & c_base_player::frozen ||
		game_rules->is_freeze_period() ||
		!c_events::is_active_round ||
		!state ||
		!config.rage.enabled)
		return;

	local->get_animation_layers()->at(7).weight = 0.f;
}

void c_antiaim::predict(c_cs_player* local, c_user_cmd* cmd)
{
	const auto state = local->get_anim_state();

	if (!local->is_local_player() || !state)
		return;

	const auto time = ticks_to_time(local->get_tick_base());

	if (local->get_velocity().length2d() >= .1f || fabsf(local->get_velocity().z) >= 100.f)
		next_lby_update = time + .22f;
	else if (time > next_lby_update)
	{
		lby_update = time;
		next_lby_update = time + 1.1f;
	}

	const auto weapon = reinterpret_cast<c_base_combat_weapon*>(
		client_entity_list()->get_client_entity_from_handle(local->get_current_weapon_handle()));

	min_delta = *reinterpret_cast<float*>(&state->pad10[512]);
	max_delta = *reinterpret_cast<float*>(&state->pad10[516]);

	float max_speed = 260.f;

	if (weapon)
	{
		const auto info = weapon_system->get_weapon_data(weapon->get_item_definition());

		if (info)
			max_speed = std::max(.001f, info->flMaxPlayerSpeed);
	}

	auto velocity = local->get_velocity();
	const auto abs_velocity_length = powf(velocity.length(), 2.f);
	const auto fraction = 1.f / (abs_velocity_length + .00000011920929f);

	if (abs_velocity_length > 97344.008f)
		velocity *= velocity * 312.f;

	auto speed = velocity.length();

	if (speed >= 260.f)
		speed = 260.f;

	feet_speed_stand = (1.923077f / max_speed) * speed;
	feet_speed_ducked = (2.9411764f / max_speed) * speed;

	auto feet_speed = (((stop_to_full_running_fraction * -.3f) - .2f) * std::clamp(feet_speed_stand, 0.f, 1.f)) + 1.f;

	if (state->duck_amount > 0.f)
		feet_speed = feet_speed + ((std::clamp(feet_speed_ducked, 0.f, 1.f) * state->duck_amount) * (.5f - feet_speed));

	min_delta *= feet_speed;
	max_delta *= feet_speed;

	if (stop_to_full_running_fraction > 0.0 && stop_to_full_running_fraction < 1.0)
	{
		const auto interval = global_vars_base->interval_per_tick * 2.f;

		if (is_standing)
			stop_to_full_running_fraction = stop_to_full_running_fraction - interval;
		else
			stop_to_full_running_fraction = interval + stop_to_full_running_fraction;

		stop_to_full_running_fraction = std::clamp(stop_to_full_running_fraction, 0.f, 1.f);
	}

	if (speed > 135.2f && is_standing)
	{
		stop_to_full_running_fraction = fmaxf(stop_to_full_running_fraction, .0099999998f);
		is_standing = false;
	}

	if (speed < 135.2f && !is_standing)
	{
		stop_to_full_running_fraction = fminf(stop_to_full_running_fraction, .99000001f);
		is_standing = true;
	}
}

float c_antiaim::get_visual_choke()
{
	return visual_choke;
}

void c_antiaim::increment_visual_progress()
{
	if (!config.rage.enabled || !engine_client()->is_ingame())
		return;

	visual_choke = 1.f;

	if (estimated_choke >= 2)
		visual_choke = static_cast<float>(client_state->choked_commands) / static_cast<float>(estimated_choke);
}

float c_antiaim::get_last_real()
{
	return last_real;
}

float c_antiaim::get_last_fake()
{
	return last_fake;
}

bool c_antiaim::on_peek(c_cs_player* local, bool& target)
{
	target = true;

	const auto weapon_cfg = c_aimhelper::get_weapon_conf();

	if (local->get_abs_velocity().length2d() < 2.f || !weapon_cfg.has_value())
		return false;

	target = false;

	const auto velocity = local->get_velocity() * (2.f / 3.f);
	const auto ticks = client_state->choked_commands > 1 ? 14 : 10;
	const auto pos = local->get_shoot_position() + velocity * ticks_to_time(ticks);

	auto found = false;

	client_entity_list()->for_each_player([&] (c_cs_player* player) -> void
	{
		if (!player->is_enemy() || player->is_dormant() || player->is_local_player())
			return;

		const auto record = animation_system->get_latest_animation(player);

		if (!record.has_value())
			return;

		const auto scan = c_ragebot::scan_record_gun(local, record.value(), pos);

		if (scan.has_value() && (scan.value().damage > weapon_cfg.value().min_dmg
				|| player->get_health() < scan.value().damage))
			found = true;
	});

	if (found)
		return true;

	target = true;
	return false;
}

float c_antiaim::calculate_ideal_yaw(c_cs_player* local, bool estimate)
{
	// step in which we test for damage.
	static constexpr auto step = 90.f;

	// maybe the name and the arguments of this function are questionable.
	const auto target = c_aimhelper::get_legit_target(28738174.56f, 0.f, c_cs_player::hitbox::body);

	if (!target.has_value())
		return engine_client()->get_view_angles().y + 180.f;
	
	const auto anim = std::get<2>(target.value());
	const auto head = anim->player->get_shoot_position();
	const auto shoot = local->get_shoot_position();
	const auto direction = math::calc_angle(local->get_origin(), anim->player->get_origin());

	// determine damage points.
	float angle_to_damage_ratio[4] = { };
	c_vector3d positions[7] = { };
	for (auto i = 0; i < 4; i++)
	{
		const auto current_angle = direction.y + i * step;
		auto& ratio = angle_to_damage_ratio[i];

		auto back_pos = math::rotate_2d(shoot, current_angle, 19.f);

		if (i == 0 || i == 2)
		{
			positions[0] = math::rotate_2d(back_pos + c_vector3d(0.f, 0.f, -6.f), current_angle + 90.f, 5.f);
			positions[1] = math::rotate_2d(back_pos + c_vector3d(0.f, 0.f, -6.f), current_angle + 90.f, -5.f);
			positions[2] = math::rotate_2d(back_pos + c_vector3d(0.f, 0.f, 0.f), current_angle + 90.f, 0.f);
			positions[3] = math::rotate_2d(back_pos + c_vector3d(0.f, 0.f, -6.f), current_angle + 90.f, 0.f);
			positions[4] = math::rotate_2d(back_pos + c_vector3d(0.f, 0.f, -6.f), current_angle + 90.f, 2.5f);
			positions[5] = math::rotate_2d(back_pos + c_vector3d(0.f, 0.f, -6.f), current_angle + 90.f, -2.5f);
			positions[6] = math::rotate_2d(back_pos + c_vector3d(0.f, 0.f, -3.f), current_angle + 90.f, 0.f);
		}
		else
		{
			positions[0] = math::rotate_2d(shoot + c_vector3d(0.f, 0.f, -6.f), current_angle, 27.f);
			positions[1] = math::rotate_2d(shoot + c_vector3d(0.f, 0.f, -6.f), current_angle, 21.f);
			positions[2] = math::rotate_2d(shoot + c_vector3d(0.f, 0.f, 0.f), current_angle, 21.f);
			positions[3] = math::rotate_2d(shoot + c_vector3d(0.f, 0.f, -7.f), current_angle, 13.f);
			positions[4] = math::rotate_2d(shoot + c_vector3d(0.f, 0.f, -6.f), current_angle, 24.f);
			positions[5] = math::rotate_2d(shoot + c_vector3d(0.f, 0.f, -6.f), current_angle, 17.f);
			positions[6] = math::rotate_2d(shoot + c_vector3d(0.f, 0.f, -3.f), current_angle, 21.f);
		}

		// the run is only for visual representation, we do not need accurate data, so we drop multipoints.
		if (estimate)
		{
			const auto wall = trace_system->wall_penetration(head, positions[2], nullptr, local);

			if (!wall.has_value())
				continue;

			if (wall.value().damage > ratio)
				ratio = wall.value().damage;

			continue;
		}

		for (auto& pos : positions)
		{
			const auto wall = trace_system->wall_penetration(head, pos, nullptr, local);

			if (!wall.has_value())
				continue;

			if (wall.value().damage > ratio)
				ratio = wall.value().damage;

			// abort if angle is already out in the open.
			if (ratio >= 60000.f)
				break;
		}
	}

	// determine lowest and highest damage.
	auto lowest_dmg = std::make_pair(0, FLT_MAX), highest_dmg = std::make_pair(0, 0.f);
	for (auto i = 3; i >= 0; i--)
	{
		const auto& ratio = angle_to_damage_ratio[i];

		// only ever face the enemy forwards if we gain a significant
		// advantage compared to all alternatives.
		if (i == 0 && lowest_dmg.second > 0.f
			&& fabsf(lowest_dmg.second - ratio) < 100.f)
			continue;

		if (lowest_dmg.second > ratio)
			lowest_dmg = std::make_pair(i, ratio);

		if (ratio > highest_dmg.second)
			highest_dmg = std::make_pair(i, ratio);
	}

	// determine second highest damage.
	auto second_highest_dmg = 0.f;
	for (auto i = 3; i >= 0; i--)
	{
		const auto& ratio = angle_to_damage_ratio[i];

		if (ratio > second_highest_dmg && highest_dmg.first != i)
			second_highest_dmg = ratio;
	}

	// no suitable point found.
	if (fabsf(lowest_dmg.second - highest_dmg.second) < 20.f && highest_dmg.second <= 40000.f)
		return engine_client()->get_view_angles().y + 180.f;

	// force backwards at target.
	if (fabsf(lowest_dmg.second - highest_dmg.second) < 100.f)
		return direction.y + 2 * step;

	// force opposite when head on edge.
	if (fabsf(second_highest_dmg - highest_dmg.second) >= 50000.f)
		return direction.y + highest_dmg.first * step + 180.f;

	// set target yaw.
	return direction.y + lowest_dmg.first * step;
}
