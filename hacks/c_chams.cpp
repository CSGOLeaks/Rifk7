#include "c_chams.h"
#include "../security/string_obfuscation.h"
#include "../sdk/c_material_system.h"
#include "../sdk/c_render_view.h"
#include "../menu/framework/macros.h"
#include "../utils/c_config.h"
#include "../sdk/c_client_entity_list.h"
#include "c_animation_system.h"
#include "../utils/math.h"
#include "../sdk/c_client_state.h"
#include "../sdk/c_prediction.h"

c_chams::c_chams() : current_player(nullptr), current_matrix(nullptr), second_pass(false),
alpha(255), direction(false) { }

void c_chams::latch_timer()
{
	if (!engine_client()->is_ingame())
		return;

	if (alpha == 0 || alpha == 255)
		direction = !direction;

	linear_fade(alpha, 0, 255, 255 / 2.5f, direction);
}

void c_chams::draw_players()
{
	if (!engine_client()->is_ingame())
		return;

	client_entity_list()->for_each_player_fixed_z_order([&] (c_cs_player* player) -> void
	{
		instance()->current_player = player;
		instance()->second_pass = false;

		const auto conf = player->is_enemy() ? config.chams.enemy : config.chams.team;

		if (conf.xqz)
		{
			instance()->second_pass = true;
			player->draw_model(1, 255);
		}

		if (player->is_enemy() && config.chams.backtrack.type)
		{
			matrix3x4 interpolated[128] = {};
			if (get_backtrack_matrix(player, interpolated))
			{
				instance()->current_matrix = interpolated;

				if (config.chams.backtrack.xqz)
				{
					instance()->second_pass = true;
					player->draw_model(1, 255);
				}

				instance()->current_matrix = nullptr;
			}
		}
	});

	instance()->current_player = nullptr;

	const auto local = c_cs_player::get_local_player();
	if (local)
	{
		instance()->second_pass = true;
		instance()->current_player = local;
		local->draw_model(1, 255);
	}

	instance()->second_pass = false;
	instance()->current_player = nullptr;
}

bool c_chams::get_backtrack_matrix(c_cs_player* player, matrix3x4* out)
{
	if (!engine_client()->is_ingame())
		return false;

	if (!net_channel)
		return false;

    auto range = config.legit.backtrack / 100.f;

	if (config.rage.enabled)
		range = 1.f;

	const auto last = animation_system->get_intermediate_animations(player, range);

	if (!last.has_value())
		return false;
    
	const auto& first_invalid = last.value().first;
	const auto& last_valid = last.value().second;

	if ((first_invalid->origin - player->get_abs_origin()).length() < 7.5f)
		return false;

	if (first_invalid->dormant)
		return false;

	if (last_valid->sim_time - first_invalid->sim_time > 0.5f)
		return false;

	const auto next = last_valid->origin;
	const auto curtime = prediction()->get_unpredicted_globals()->curtime;

	auto delta = 1.f - (curtime - last_valid->interp_time) / (last_valid->sim_time - first_invalid->sim_time);
	if (delta < 0.f || delta > 1.f)
		last_valid->interp_time	= curtime;

	delta = 1.f - (curtime - last_valid->interp_time) / (last_valid->sim_time - first_invalid->sim_time);

	const auto lerp = math::interpolate(next, first_invalid->origin, std::clamp(delta, 0.f, 1.f));

	matrix3x4 ret[128];
	memcpy(ret, first_invalid->bones, sizeof(matrix3x4[128]));

	for (size_t i{}; i < 128; ++i)
	{
		const auto matrix_delta = math::matrix_get_origin(first_invalid->bones[i]) - first_invalid->origin;
		math::matrix_set_origin(matrix_delta + lerp, ret[i]);
	}

	memcpy(out, ret, sizeof(matrix3x4[128]));
	return true;
}

void c_chams::player_chams(const std::function<void()> original, c_config::config_conf::chams_conf::chams& conf)
{
	if (!conf.type)
		return;

	static const auto glow = find_mat("dev/glow_armsrace");
	static const auto cube = find_mat("debug/debugambientcube");
	static const auto glass = find_mat("models/gibs/glass/glass");
	static const auto skull = find_mat("models/gibs/hgibs/skull1");

	const auto alpha = conf.type == 1 ? instance()->alpha : 255;

	set_ignorez(instance()->second_pass ? conf.xqz : false);

	if (conf.type == 1)
	{
		if (!instance()->current_matrix)
			original();

		modulate(conf.color, glow);
		modulate_exp(glow, alpha / 255.f, 60.f);
		model_render()->forced_material_override(glow);

		if (instance()->second_pass && conf.xqz)
		{
			original();
			if (!instance()->current_matrix)
			{
				set_ignorez(false);
				render_view()->set_color_modulation(c_color(255, 255, 255));
				model_render()->forced_material_override(nullptr);
				original();
			}
			modulate(conf.color, glow);
			modulate_exp(glow, alpha / 255.f, 60.f);
			model_render()->forced_material_override(glow);
		}
	}
	else
	{
		const auto material = conf.type == 2 ? cube : conf.type == 3 ? skull : glass;

		modulate(conf.color, material);
		model_render()->forced_material_override(material);
		original();

		modulate(c_color::foreground(), glow);
		modulate_exp(glow, alpha / 255.f, 60.f);
		model_render()->forced_material_override(glow);
		original();

		if (instance()->second_pass && conf.xqz)
		{
			set_ignorez(false);
			modulate(conf.color, material);
			model_render()->forced_material_override(material);
			original();
			modulate(c_color::foreground(), glow);
			modulate_exp(glow, alpha / 255.f, 60.f);
			model_render()->forced_material_override(glow);
		}
	}
}

void c_chams::viewmodel_chams(const std::function<void()> original, c_config::config_conf::chams_conf::chams& conf)
{
	if (!conf.type)
		return;

	static const auto glow = find_mat("dev/glow_armsrace");
	static const auto fbi = find_mat("models/player/ct_fbi/ct_fbi_glass");
	static const auto tags = find_mat("models/inventory_items/dogtags/dogtags_lightray");

	const auto alpha = conf.type == 1 || conf.type == 4 ? instance()->alpha : 255;

	if (conf.type == 1)
	{
		original();
		modulate(conf.color, glow);
		modulate_exp(glow, alpha / 255.f);
		model_render()->forced_material_override(glow);
	}
	else
	{
		modulate(c_color::background(), fbi);
		model_render()->forced_material_override(fbi);
		original();

		render_view()->set_blend(alpha / 255.f);
		tags->set_material_var_flag(material_var_wireframe, true);
		modulate(conf.color, tags);
		model_render()->forced_material_override(tags);

		if (conf.type > 2)
		{
			original();
			render_view()->set_blend(1.f);
			modulate(conf.color, glow);
			modulate_exp(glow, alpha / 255.f);
			model_render()->forced_material_override(glow);
		}
	}
}

void c_chams::modulate(const c_color color, c_material* material)
{
	if (material)
	{
		material->modulate(color);

		const auto tint = material->find_var(_("$envmaptint"));
		if (tint)
			tint->set_vector(c_vector3d(color.red / 255.f, color.green / 255.f, color.blue / 255.f));
	}

	render_view()->set_color_modulation(color);
}

void c_chams::modulate_exp(c_material* material, const float alpha, const float width)
{
	const auto transform = material->find_var(_("$envmapfresnelminmaxexp"));
	if (transform)
		transform->set_vector_component(width * alpha, 1);
}

void c_chams::set_ignorez(const bool enabled)
{
	static const auto glow = find_mat("dev/glow_armsrace");
	static const auto cube = find_mat("debug/debugambientcube");
	static const auto glass = find_mat("models/gibs/glass/glass");
	static const auto skull = find_mat("models/gibs/hgibs/skull1");

	glow->set_material_var_flag(material_var_ignorez, enabled);
	cube->set_material_var_flag(material_var_ignorez, enabled);
	glass->set_material_var_flag(material_var_ignorez, enabled);
	skull->set_material_var_flag(material_var_ignorez, enabled);
}
