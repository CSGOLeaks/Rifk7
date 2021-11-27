#include "c_legitbot.h"
#include "../utils/math.h"
#include "../sdk/c_global_vars_base.h"
#include "../sdk/c_cvar.h"
#include "c_animation_system.h"
#include "c_aimhelper.h"
#include "../hooks/idirect3ddevice9.h"

void c_legitbot::aim(float* x, float* y)
{
	if (!config.legit.assist || config.rage.enabled)
		return;

	auto local = c_cs_player::get_local_player();

	if (!local)
		return;

	const auto weapon = client_entity_list()->get_client_entity_from_handle(local->get_current_weapon_handle());

	if (!weapon)
		return;

	static std::optional<std::tuple<c_vector3d, float, c_animation_system::animation*>> target = std::nullopt;
	static auto time = .0f;

	if (!target.has_value() || time > .15f)
	{
		const auto hitbox = weapon->get_class_id() == cweaponawp ? c_cs_player::hitbox::pelvis : c_cs_player::hitbox::head;
		target = c_aimhelper::get_legit_target(config.legit.fov, config.legit.backtrack / 100.f, hitbox, true);
		time = 0.f;
	}

	if (!target.has_value())
		return;

	time += global_vars_base->frametime;

	c_qangle delta;
	const auto view_angle = engine_client()->get_view_angles();

	const auto shoot_position = local->get_shoot_position();

	const auto recoil_scale = cvar()->find_var(_("weapon_recoil_scale"))->get_float();
	auto aim_angle = math::calc_angle(shoot_position, std::get<0>(target.value()));
	aim_angle -= local->get_punch_angle() * recoil_scale;

	smooth(aim_angle, view_angle);
	const auto move_angle = pixels_to_angle({ *x, *y });
	auto view_delta = aim_angle - view_angle;
	math::normalize(view_delta);

	const auto delta_x = std::abs(move_angle.x);
	const auto delta_y = std::abs(move_angle.y);

	delta.x = std::clamp(view_delta.x, -delta_x, delta_x);
	delta.y = std::clamp(view_delta.y, -delta_y, delta_y);

	const auto pixels = angle_to_pixels(delta);
	*x += pixels.x;
	*y += pixels.y;
}

void c_legitbot::backtrack(c_cs_player* local, c_user_cmd* cmd)
{
	if (config.rage.enabled
		|| config.legit.backtrack <= 0.f
		|| !(cmd->buttons & c_user_cmd::flags::attack)
		|| !local->can_shoot(cmd, global_vars_base->curtime, true))
		return;

	const auto target = c_aimhelper::get_legit_target(config.legit.fov, config.legit.backtrack / 100.f);

	if (!target.has_value())
		return;

	cmd->tick_count = time_to_ticks(std::get<1>(target.value())) + lerp_ticks;
}

void c_legitbot::smooth(c_qangle& end_angles, const c_qangle& prev_angles)
{
	static auto cur_aim_time = 0.f;
	auto delta = end_angles - prev_angles;
	math::normalize(delta);

	if (delta.length() == 0.f)
		return;

	const float random = rnd_flt(0.f, .1f);
	const auto final_time = delta.length() / ((3.f + random) / 10.f);
	cur_aim_time += global_vars_base->frametime;

	if (cur_aim_time > 0.12f)
		cur_aim_time = 0.11f;

	if (cur_aim_time > final_time)
		cur_aim_time = final_time;

	delta *= cur_aim_time / final_time;

	const float xrand = rnd_flt(0.f, 0.6f * config.legit.smooth / 100.f);
	const float yrand = rnd_flt(0.f, 0.4f * config.legit.smooth / 100.f);
	delta.x += delta.x / (5.f + xrand);
	delta.y += delta.y / (2.f + yrand);

	end_angles = prev_angles + delta;
	math::normalize(end_angles);
}

c_vector2d c_legitbot::angle_to_pixels(const c_qangle& angle)
{
	const auto m_yaw = cvar()->find_var(_("m_yaw"))->get_float();
	const auto m_pitch = cvar()->find_var(_("m_pitch"))->get_float();
	return c_vector2d(-angle.y / m_yaw, angle.x / m_pitch);
}

c_qangle c_legitbot::pixels_to_angle(const c_vector2d& pixel)
{
	const auto m_yaw = cvar()->find_var(_("m_yaw"))->get_float();
	const auto m_pitch = cvar()->find_var(_("m_pitch"))->get_float();

	auto result = c_qangle(-pixel.y * m_yaw, pixel.x * m_pitch, 0.f);
	math::normalize(result);
	return result;
}
