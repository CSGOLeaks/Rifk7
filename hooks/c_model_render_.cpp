#include "c_model_render_.h"
#include "../utils/c_hook.h"
#include "../sdk/c_model_info_client.h"
#include "../hacks/c_chams.h"
#include "../sdk/c_render_view.h"
#include "../utils/c_config.h"
#include "../sdk/c_cs_player.h"
#include "../sdk/c_input.h"
#include "../sdk/c_client_entity_list.h"
#include "../utils/c_log.h"
#include "../hacks/c_animation_system.h"

void c_model_render_::hook()
{
	static c_hook<c_model_render> hook(model_render());
	_draw_model_execute = hook.apply<draw_model_execute_t>(21, draw_model_execute);
}

void c_model_render_::draw_model_execute(c_model_render* model_render, uint32_t, void* ctx, draw_model_state* state, model_render_info_t& info, matrix3x4* matrix)
{
	if (!engine_client()->is_ingame())
		return _draw_model_execute(model_render, ctx, state, info, matrix);

	render_view()->set_color_modulation(c_color(255, 255, 255));

	const auto entity = reinterpret_cast<c_cs_player*>(client_entity_list()->get_client_entity(info.entity_index));

	if (!info.model)
		return _draw_model_execute(model_render, ctx, state, info, matrix);

	const auto original = [&]() -> void
	{
		_draw_model_execute(model_render, ctx, state, info,
			c_chams::instance()->current_matrix ? c_chams::instance()->current_matrix : matrix);
	};

	if (strstr(info.model->name, _("models/player")) != nullptr)
	{
		if (entity && !entity->is_ragdoll())
		{
			render_view()->set_blend(entity->is_local_player() && entity->is_scoped()
				&& config.misc.no_scope	&& input->camera_in_third_person ? .6f : 1.f);

			if (!c_chams::instance()->second_pass && c_chams::instance()->current_player == entity && !entity->is_local_player())
				return;

			if (!entity->is_local_player())
			{
				if (entity->is_enemy() && config.chams.backtrack.type && !c_chams::instance()->current_matrix)
				{
					matrix3x4 interpolated[128] = {};
					if (c_chams::get_backtrack_matrix(entity, interpolated))
					{
						c_chams::instance()->current_matrix = interpolated;
						c_chams::player_chams(original, config.chams.backtrack);
						original();
						render_view()->set_color_modulation(c_color(255, 255, 255));
						model_render->forced_material_override(nullptr);
						c_chams::set_ignorez(false);
						c_chams::instance()->current_matrix = nullptr;
					}
				}

				if (c_chams::instance()->current_matrix)
					c_chams::player_chams(original, config.chams.backtrack);
				else
					c_chams::player_chams(original, entity->is_enemy() ? config.chams.enemy : config.chams.team);
			}
			else
				c_chams::player_chams(original, config.chams.local);
		}
	}

	if (strstr(info.model->name, _("sleeve")) && config.chams.remove_sleeves)
		return; // skip sleeves
	// fuck you too, valve.
	else if (strstr(info.model->name, _("arms")) != nullptr || fnv1a_rt(info.model->name) == 2565982016)
		c_chams::viewmodel_chams(original, config.chams.arms);
	else if (strstr(info.model->name, _("weapons/v_")) != nullptr)
	{
		const auto local = c_cs_player::get_local_player();

		if (local && local->is_scoped() && config.chams.weapon.type)
			state->draw_flags |= studiorender_draw_translucent_only;

		c_chams::viewmodel_chams(original, config.chams.weapon);
	}

	original();
	render_view()->set_blend(1.f);
	model_render->forced_material_override(nullptr);
	c_chams::set_ignorez(false);
}
