#include "c_animation_system.h"
#include "c_trace_system.h"
#include "c_antiaim.h"
#include "../sdk/c_global_vars_base.h"
#include "../sdk/c_prediction.h"
#include "../sdk/c_client_entity_list.h"
#include "../sdk/c_cvar.h"
#include "../utils/math.h"
#include "c_resolver.h"
#include "c_esp.h"
#include "../sdk/c_input.h"

c_animation_system::animation::animation(c_cs_player* player)
{
	const auto weapon = reinterpret_cast<c_base_combat_weapon*>(
		client_entity_list()->get_client_entity_from_handle(player->get_current_weapon_handle()));

	this->player = player;
	index = player->index();
	dormant = player->is_dormant();
	velocity = player->get_velocity();
	origin = player->get_origin();
	abs_origin = player->get_abs_origin();
	obb_mins = player->get_mins();
	obb_maxs = player->get_maxs();
	layers = *player->get_animation_layers();
	poses = player->get_pose_parameter();
	if ((has_anim_state = player->get_anim_state()))
		anim_state = *player->get_anim_state();
	anim_time = player->get_old_simtime() + global_vars_base->interval_per_tick;
	sim_time = player->get_simtime();
	interp_time = 0.f;
	last_shot_time = weapon ? weapon->get_last_shot_time() : 0.f;
	duck = player->get_duck_amount();
	lby = player->get_lby();
	eye_angles = player->get_eye_angles();
	abs_ang = player->get_abs_angles();
	flags = player->get_flags();
	eflags = player->get_eflags();
	effects = player->get_effects();
	lag = time_to_ticks(player->get_simtime() - player->get_old_simtime());

	// animations are off when we enter pvs, we do not want to shoot yet.
	valid = lag >= 0 && lag <= 17;

	// clamp it so we don't interpolate too far : )
	lag = std::clamp(lag, 0, 17);
}

c_animation_system::animation::animation(c_cs_player* player, c_qangle last_reliable_angle) : animation(player)
{
	this->last_reliable_angle = last_reliable_angle;
}

void c_animation_system::animation::restore(c_cs_player* player) const
{
	player->get_velocity() = velocity;
	player->get_flags() = flags;
	player->get_eflags() = eflags;
	player->get_duck_amount() = duck;
	*player->get_animation_layers() = layers;
	player->get_lby() = lby;
	player->get_origin() = origin;
	player->set_abs_origin(abs_origin);
}

void c_animation_system::animation::apply(c_cs_player* player) const
{
	player->get_pose_parameter() = poses;
	player->get_eye_angles() = eye_angles;
	player->get_velocity() = player->get_abs_velocity() = velocity;
	player->get_lby() = lby;
	player->get_duck_amount() = duck;
	player->get_flags() = flags;
	player->get_origin() = origin;
	player->set_abs_origin(origin);
	if (player->get_anim_state() && has_anim_state)
		*player->get_anim_state() = anim_state;
}

void c_animation_system::animation::build_server_bones(c_cs_player* player)
{
	// keep track of old occlusion values
	const auto backup_occlusion_flags = player->get_occlusion_flags();
	const auto backup_occlusion_framecount = player->get_occlusion_framecount();

	// skip occlusion checks in c_cs_player::setup_bones
	if (!player->is_local_player())
	{
		player->get_occlusion_flags() = 0;
		player->get_occlusion_framecount() = 0;
	}

	// clear bone accessor
	player->get_readable_bones() = player->get_writable_bones() = 0;

	// invalidate bone cache
	player->get_most_recent_model_bone_counter() = 0;
	player->get_last_bone_setup_time() = -FLT_MAX;

	// stop interpolation
	player->get_effects() |= c_base_entity::ef_nointerp;

	// change bone accessor
	const auto backup_bone_array = player->get_bone_array_for_write();
	if (!player->is_local_player())
		player->get_bone_array_for_write() = bones;

	// build bones
	player->setup_bones(nullptr, -1, bone_used_by_anything, global_vars_base->curtime);

	// restore bone accessor
	player->get_bone_array_for_write() = backup_bone_array;

	// restore original occlusion
	if (!player->is_local_player())
	{
		player->get_occlusion_flags() = backup_occlusion_flags;
		player->get_occlusion_framecount() = backup_occlusion_framecount;
	}
	
	// start interpolation again
	player->get_effects() &= ~c_base_entity::ef_nointerp;
}

void c_animation_system::animation_info::update_animations(animation* record, animation* from)
{
	if (!from)
	{
		// set velocity and layers.
		record->velocity = player->get_velocity();

		// fix feet spin.
		record->anim_state.feet_yaw_rate = 0.f;

		// resolve player.
		c_resolver::resolve(record);

		// apply record.
		record->apply(player);

		// run update.
		return instance()->update_player(player);
	}

	const auto new_velocity = player->get_velocity();

	// restore old record.
	*player->get_animation_layers() = from->layers;
	player->set_abs_origin(record->origin);
	player->set_abs_angles(from->abs_ang);
	player->get_velocity() = from->velocity;

	// setup velocity.
	record->velocity = new_velocity;

	// did the player shoot?
	const auto shot = record->last_shot_time > from->sim_time && record->last_shot_time <= record->sim_time;

	// setup extrapolation parameters.
	auto old_origin = from->origin;
	auto old_flags = from->flags;

	for (auto i = 0; i < record->lag; i++)
	{
		// move time forward.
		const auto time = from->sim_time + ticks_to_time(i + 1);
		const auto lerp = 1.f - (record->sim_time - time) / (record->sim_time - from->sim_time);

		// lerp eye angles.
		auto eye_angles = math::interpolate(from->eye_angles, record->eye_angles, lerp);
		math::normalize(eye_angles);
		player->get_eye_angles() = eye_angles;

		// lerp duck amount.
		player->get_duck_amount() = math::interpolate(from->duck, record->duck, lerp);

		// resolve player.
		if (record->lag - 1 == i)
		{
			player->get_velocity() = new_velocity;
			player->get_flags() = record->flags;

			if (record->lag > 1)
				c_resolver::resolve(record);
		}
		else // compute velocity and flags.
		{
			c_trace_system::extrapolate(player, old_origin, player->get_velocity(), player->get_flags(), old_flags & c_base_player::on_ground);
			old_flags = player->get_flags();
		}

		// correct shot desync.
		if (shot)
		{
			player->get_eye_angles() = record->last_reliable_angle;

			if (record->last_shot_time <= time)
				player->get_eye_angles() = record->eye_angles;
		}

		// instant approach.
		if (player->get_velocity().length2d() < .1f && fabsf(player->get_velocity().z) < 100.f && record->lag > 1)
			animation_system->last_process_state = player->get_anim_state();

		// fix feet spin.
		player->get_anim_state()->feet_yaw_rate = 0.f;

		// backup simtime.
		const auto backup_simtime = player->get_simtime();

		// set new simtime.
		player->get_simtime() = time;

		// run update.
		instance()->update_player(player);

		// restore old simtime.
		player->get_simtime() = backup_simtime;
	}
}

void c_animation_system::post_player_update()
{
	if (!engine_client()->is_ingame())
		return;

	const auto local = c_cs_player::get_local_player();

	// erase outdated entries
	for (auto it = animation_infos.begin(); it != animation_infos.end();) {
		auto player = reinterpret_cast<c_cs_player*>(client_entity_list()->get_client_entity_from_handle(it->first));

		if (!player || player != it->second.player || !player->is_alive() || !player->is_enemy()
			|| !local || !local->is_alive())
		{
			if (player)
				player->get_client_side_animation() = true;
			it = animation_infos.erase(it);
		}
		else
			it = next(it);
	}

	if (!local || !local->is_alive())
	{
		client_entity_list()->for_each_player([](c_cs_player* player) -> void
		{
			player->get_client_side_animation() = true;
		});
		return;
	}
	
	// create new entries and reset old ones
	client_entity_list()->for_each_player([&](c_cs_player* player) -> void {
		if (!player->is_enemy() && !player->is_local_player())
			player->get_client_side_animation() = true;

		if (!player->is_alive() || player->is_dormant() || player->is_local_player() || !player->is_enemy())
			return;

		if (animation_infos.find(player->get_handle()) == animation_infos.end())
			animation_infos.insert_or_assign(player->get_handle(), animation_info(player, {}));
	});

	// run post update
	for (auto& info : animation_infos)
	{
		auto& animation = info.second;
		const auto player = animation.player;

		// erase frames out-of-range
		for (auto it = animation.frames.rbegin(); it != animation.frames.rend();) {
			if (global_vars_base->curtime - it->sim_time > 1.2f)
				it = decltype(it) { info.second.frames.erase(next(it).base()) };
			else
				it = next(it);
		}

		// have we already seen this update?
		if (player->get_simtime() == player->get_old_simtime())
			continue;

		// reset animstate
		if (animation.last_spawn_time != player->get_spawn_time())
		{
			const auto state = player->get_anim_state();
			if (state)
				state->reset();

			animation.last_spawn_time = player->get_spawn_time();
		}

		// grab weapon
		const auto weapon = reinterpret_cast<c_base_combat_weapon*>(
			client_entity_list()->get_client_entity_from_handle(player->get_current_weapon_handle()));

		// make a full backup of the player
		auto backup = c_animation_system::animation(player);
		backup.apply(player);

		// grab previous
		c_animation_system::animation* previous = nullptr;

		if (!animation.frames.empty() && !animation.frames.front().dormant
			&& time_to_ticks(player->get_simtime() - animation.frames.front().sim_time) <= 17)
			previous = &animation.frames.front();

		// update shot info
		const auto shot = weapon && previous && weapon->get_last_shot_time() > previous->sim_time
			&& weapon->get_last_shot_time() <= player->get_simtime();

		if (!shot)
			info.second.last_reliable_angle = player->get_eye_angles();

		// store server record
		auto& record = animation.frames.emplace_front(player, info.second.last_reliable_angle);

		// run full update
		animation.update_animations(&record, previous);

		// restore server layers
		*player->get_animation_layers() = backup.layers;

		// use uninterpolated data to generate our bone matrix
		record.build_server_bones(player);

		// restore correctly synced values
		backup.restore(player);
		
		// set record to latest animation
		animation.latest_animation = record;
	}
}

void c_animation_system::update_player(c_cs_player* player)
{
	static auto& enable_bone_cache_invalidation = **reinterpret_cast<bool**>(
		reinterpret_cast<uint32_t>(sig("client_panorama.dll", "C6 05 ? ? ? ? ? 89 47 70")) + 2);

	// make a backup of globals
	const auto backup_frametime = global_vars_base->frametime;
	const auto backup_curtime = global_vars_base->curtime;

	// get player anim state
	const auto state = player->get_anim_state();

	// fixes for networked players
	if (!player->is_local_player())
	{
		global_vars_base->frametime = global_vars_base->interval_per_tick;
		global_vars_base->curtime = player->get_simtime();
		player->get_eflags() &= ~c_base_entity::efl_dirty_absvelocity;
	}

	// allow reanimating in the same frame
	if (state->last_client_side_animation_update_framecount == global_vars_base->framecount)
		state->last_client_side_animation_update_framecount -= 1;

	// make sure we keep track of the original invalidation state
	const auto old_invalidation = enable_bone_cache_invalidation;

	// notify the other hooks to instruct animations and pvs fix
	instance()->enable_bones = player->get_client_side_animation() = true;
	player->update_clientside_anim();
	instance()->enable_bones = false;

	if (!player->is_local_player())
		player->get_client_side_animation() = false;

	instance()->last_process_state = nullptr;

	// invalidate physics.
	if (!player->is_local_player())
		player->invalidate_physics_recursive(c_base_entity::angles_changed
			| c_base_entity::animation_changed
			| c_base_entity::sequence_changed);
	
	// we don't want to enable cache invalidation by accident
	enable_bone_cache_invalidation = old_invalidation;

	// restore globals
	global_vars_base->frametime = backup_frametime;
	global_vars_base->curtime = backup_curtime;
}

void c_animation_system::update_simple_local_player(c_cs_player* player, c_user_cmd* cmd)
{
	player->get_pose_parameter() = animation_system->local_animation.poses;
	*player->get_animation_layers() = animation_system->server_layers;
	animation_system->local_animation.eye_angles = cmd->viewangles;
	math::normalize(animation_system->local_animation.eye_angles);

	player->get_anim_state()->feet_yaw_rate = 0.f;
	animation_system->update_player(player);
	animation_system->local_animation.abs_ang.y = player->get_anim_state()->goal_feet_yaw;
	animation_system->local_animation.layers = *player->get_animation_layers();
	animation_system->local_animation.poses = player->get_pose_parameter();
}

c_animation_system::animation_info* c_animation_system::get_animation_info(c_cs_player* player)
{
	auto info = animation_infos.find(player->get_handle());

	if (info == animation_infos.end())
		return nullptr;

	return &info->second;
}

std::optional<c_animation_system::animation*> c_animation_system::get_latest_animation(c_cs_player* player)
{
	const auto info = animation_infos.find(player->get_handle());

	if (info == animation_infos.end() || info->second.frames.empty())
		return std::nullopt;

	for (auto it = info->second.frames.begin(); it != info->second.frames.end(); it = next(it))
		if (it->is_valid())
			return &*it;

	return std::nullopt;
}

std::optional<c_animation_system::animation*> c_animation_system::get_oldest_animation(c_cs_player* player)
{
	const auto info = animation_infos.find(player->get_handle());

	if (info == animation_infos.end() || info->second.frames.empty())
		return std::nullopt;

	for (auto it = info->second.frames.rbegin(); it != info->second.frames.rend(); it = next(it))
		if (it->is_valid())
			return &*it;

	return std::nullopt;
}

std::optional<c_animation_system::animation*> c_animation_system::get_uncrouched_animation(c_cs_player * player)
{
	const auto info = animation_infos.find(player->get_handle());

	if (info == animation_infos.end() || info->second.frames.empty())
		return std::nullopt;

	for (auto it = info->second.frames.begin(); it != info->second.frames.end(); it = next(it))
		if (it->is_valid() && fabsf(it->duck) < .0001f)
			return &*it;

	return std::nullopt;
}

std::optional<std::pair<c_animation_system::animation*, c_animation_system::animation*>> c_animation_system::get_intermediate_animations(
	c_cs_player* player, const float range)
{
	const auto info = animation_infos.find(player->get_handle());

	if (info == animation_infos.end() || info->second.frames.empty())
		return std::nullopt;

	for (auto it = info->second.frames.begin(); it != info->second.frames.end(); it = next(it))
		if (it->is_valid(range * .2f) && it + 1 != info->second.frames.end() && !(it + 1)->is_valid(range * .2f))
			return std::make_pair(&*(it + 1), &*it);

	return std::nullopt;
}

std::vector<c_animation_system::animation*> c_animation_system::get_valid_animations(c_cs_player* player, const float range)
{
	std::vector<animation*> result;

	const auto info = animation_infos.find(player->get_handle());

	if (info == animation_infos.end() || info->second.frames.empty())
		return result;

	result.reserve(static_cast<int>(std::ceil(range * .2f / global_vars_base->interval_per_tick)));

	for (auto it = info->second.frames.begin(); it != info->second.frames.end(); it = next(it))
		if (it->is_valid(range * .2f))
			result.push_back(&*it);

	return result;
}

std::optional<c_animation_system::animation*> c_animation_system::get_lastest_animation_unsafe(c_cs_player* player)
{
	const auto latest = get_latest_animation(player);

	if (latest.has_value())
		return latest;

	const auto info = animation_infos.find(player->get_handle());

	if (info == animation_infos.end())
		return std::nullopt;

	return &info->second.latest_animation;
}
