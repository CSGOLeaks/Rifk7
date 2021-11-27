#include "c_resolver.h"
#include "../utils/math.h"
#include "c_aimhelper.h"
#include "c_trace_system.h"
#include "../sdk/c_client_entity_list.h"
#include <random>
#include "c_esp.h"

static std::random_device rd;
static std::mt19937 rng(rd());

void c_resolver::resolve(c_animation_system::animation* anim)
{
	const auto info = animation_system->get_animation_info(anim->player);

	if (!info || !anim->has_anim_state || anim->player->get_info().fakeplayer)
		return;
	
	if (info->brute_state == resolver_start)
		info->brute_yaw = std::uniform_int_distribution<int>(0, 1)(rng) ? -120.f : 120.f;

	anim->anim_state.goal_feet_yaw = math::normalize_yaw(anim->anim_state.goal_feet_yaw + info->brute_yaw);
}

void c_resolver::resolve_shot(resolver::shot& shot)
{
	if (!config.rage.enabled || shot.manual)
		return;

	const auto player = reinterpret_cast<c_cs_player*>(client_entity_list()->get_client_entity(shot.record.index));

	if (player != shot.record.player)
		return;

	const auto hdr = model_info_client()->get_studio_model(shot.record.player->get_model());

	if (!hdr)
		return;

	const auto info = animation_system->get_animation_info(player);

	if (!info)
		return;

	const auto angle = math::calc_angle(shot.start, shot.server_info.impacts.back());
	c_vector3d forward;
	math::angle_vectors(angle, forward);
	const auto end = shot.server_info.impacts.back() + forward * 2000.f;
	const auto spread_miss = !c_aimhelper::can_hit_hitbox(shot.start, end, &shot.record, hdr, shot.hitbox);

	if (shot.server_info.damage > 0)
	{
		static const auto hit_msg = __("Hit %s in %s for %d damage.");
		_rt(hit, hit_msg);
		char msg[255];

		switch (shot.server_info.hitgroup)
		{
		case hitgroup_head:
			sprintf_s(msg, hit, player->get_info().name, _("head"), shot.server_info.damage);
			break;
		case hitgroup_leftleg:
		case hitgroup_rightleg:
			sprintf_s(msg, hit, player->get_info().name, _("leg"), shot.server_info.damage);
			break;
		case hitgroup_stomach:
			sprintf_s(msg, hit, player->get_info().name, _("stomach"), shot.server_info.damage);
			break;
		default:
			sprintf_s(msg, hit, player->get_info().name, _("body"), shot.server_info.damage);
			break;
		}

		logging->info(msg);
	}
	else if (spread_miss)
	{
		logging->info(_("Missed shot due to spread."));
		++info->missed_due_to_spread;
	}

	if (!shot.record.player->is_alive() || shot.record.player->get_info().fakeplayer
		|| !shot.record.has_anim_state || !shot.record.player->get_anim_state() || !info)
		return;

	// note old brute_yaw.
	const auto old_brute_yaw = info->brute_yaw;

	// check deviation from server.
	auto backup = c_animation_system::animation(shot.record.player);
	shot.record.apply(player);
	const auto trace = trace_system->wall_penetration(shot.start, end, &shot.record);
	auto does_match = (trace.has_value() && trace.value().hitgroup == shot.server_info.hitgroup)
		|| (!trace.has_value() && spread_miss);

	// start brute.
	if (!does_match)
	{
		switch (info->brute_state)
		{
		case resolver_start:
			info->brute_state = resolver_inverse;
			info->brute_yaw = -info->brute_yaw;
			logging->debug("BRUTE: START -> INVERSE");
			break;
		case resolver_inverse:
			info->brute_state = resolver_no_desync;
			logging->debug("BRUTE: INVERSE -> NONE");
			info->brute_yaw = 0.f;
			break;
		case resolver_no_desync:
			info->brute_state = resolver_jitter;
			info->brute_yaw = std::uniform_int_distribution<int>(0, 1)(rng) ? -120.f : 120.f;
			logging->debug("BRUTE: NONE -> JITTER");
			break;
		default:
		case resolver_jitter:
			info->brute_yaw = -info->brute_yaw;
			logging->debug("BRUTE: JITTER -> JITTER");
			break;
		}
	}

	// apply changes.
	if (!info->frames.empty())
	{
		c_animation_system::animation* previous = nullptr;
		
		// jump back to the beginning.
		*player->get_anim_state() = info->frames.back().anim_state;

		for (auto it = info->frames.rbegin(); it != info->frames.rend(); ++it)
		{
			auto& frame = *it;
			const auto frame_player = reinterpret_cast<c_cs_player*>(
				client_entity_list()->get_client_entity(frame.index));

			if (frame_player == frame.player
				&& frame.player == player)
			{
				// re-run complete animation code and repredict all animations in between!
				frame.anim_state = *player->get_anim_state();
				frame.apply(player);
				player->get_flags() = frame.flags;
				*player->get_animation_layers() = frame.layers;
				player->get_simtime() = frame.sim_time;

				info->update_animations(&frame, previous);
				frame.abs_ang.y = player->get_anim_state()->goal_feet_yaw;
				frame.flags = player->get_flags();
				*player->get_animation_layers() = frame.layers;
				frame.build_server_bones(player);
				previous = &frame;
			}
		}
	}
}

void c_resolver::register_shot(resolver::shot&& s)
{
	shots.emplace_front(std::move(s));
}

void c_resolver::on_player_hurt(c_game_event* event)
{
	const auto attacker = event->get_int(_("attacker"));
	const auto attacker_index = engine_client()->get_player_for_user_id(attacker);

	if (attacker_index != engine_client()->get_local_player())
		return;

	if (shots.empty())
		return;

	resolver::shot* last_confirmed = nullptr;

	for (auto it = shots.rbegin(); it != shots.rend(); it = next(it))
	{
		if (it->confirmed && !it->skip)
		{
			last_confirmed = &*it;
			break;
		}
	}

	if (!last_confirmed)
		return;

	const auto userid = event->get_int(_("userid"));
	const auto index = engine_client()->get_player_for_user_id(userid);

	if (index != last_confirmed->record.index)
		return;

	last_confirmed->server_info.index = index;
	last_confirmed->server_info.damage = event->get_int(_("dmg_health"));
	last_confirmed->server_info.hitgroup = event->get_int(_("hitgroup"));
}

void c_resolver::on_bullet_impact(c_game_event* event)
{
	const auto userid = event->get_int(_("userid"));
	const auto index = engine_client()->get_player_for_user_id(userid);

	if (index != engine_client()->get_local_player())
		return;

	if (shots.empty())
		return;

	resolver::shot* last_confirmed = nullptr;

	for (auto it = shots.rbegin(); it != shots.rend(); it = next(it))
	{
		if (it->confirmed && !it->skip)
		{
			last_confirmed = &*it;
			break;
		}
	}

	if (!last_confirmed)
		return;

	last_confirmed->impacted = true;
	last_confirmed->server_info.impacts.emplace_back(event->get_float(_("x")),
		event->get_float(_("y")),
		event->get_float(_("z")));
}

void c_resolver::on_weapon_fire(c_game_event* event)
{
	const auto userid = event->get_int(_("userid"));
	const auto index = engine_client()->get_player_for_user_id(userid);

	if (index != engine_client()->get_local_player())
		return;

	if (shots.empty())
		return;

	resolver::shot* last_unconfirmed = nullptr;

	for (auto it = shots.rbegin(); it != shots.rend(); it = next(it))
	{
		if (!it->confirmed)
		{
			last_unconfirmed = &*it;
			break;
		}

		it->skip = true;
	}

	if (!last_unconfirmed)
		return;

	last_unconfirmed->confirmed = true;
}

void c_resolver::on_render_start()
{
	for (auto it = shots.begin(); it != shots.end();)
	{
		if (it->time + 1.f < global_vars_base->curtime)
			it = shots.erase(it);
		else
			it = next(it);
	}

	for (auto it = shots.begin(); it != shots.end();)
	{
		if (it->confirmed && it->impacted)
		{
			resolve_shot(*it);
			c_esp::draw_local_impact(it->start, it->server_info.impacts.back());
			it = shots.erase(it);
		}	
		else
			it = next(it);
	}
}

