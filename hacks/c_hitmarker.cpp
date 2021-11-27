#include "c_hitmarker.h"
#include "../sdk/c_client_entity_list.h"
#include "../BASS/API.h"
#include "../hooks/idirect3ddevice9.h"

int c_hitmarker::hit_alpha = 0;

void c_hitmarker::draw()
{
	linear_fade(hit_alpha, 0, 235, 255 / 1.5f, false);

	if (hit_alpha <= 0)
		return;

	c_color::set_alpha_override(hit_alpha);
	renderer->triangle_linear_gradient(renderer->get_center() + c_vector2d(-30, 20),
									   renderer->get_center() + c_vector2d(30, 20),
									   renderer->get_center() - c_vector2d(0, 30),
									   c_color::primary(), c_color::accent(), c_color::accent());
	c_color::set_alpha_override(std::nullopt);
}

void c_hitmarker::on_player_hurt(c_game_event* event)
{
	if (!config.esp.hitmarker)
		return;

	auto local = c_cs_player::get_local_player();
	if (!local || !local->is_alive())
		return;

	const auto attacker = client_entity_list()->get_client_entity(engine_client()->get_player_for_user_id(event->get_int(_("attacker"))));
	const auto target = reinterpret_cast<c_cs_player*>(client_entity_list()->get_client_entity(
		engine_client()->get_player_for_user_id(event->get_int(_("userid")))));

	if (attacker && target && attacker == local && target->is_enemy())
	{
		hit_alpha = 255;

		uint32_t hitsound = 0;

		switch (config.esp.hitsound)
		{
		case 1:
			hitsound = BASS::stream_sounds.cod_hitsound;
			break;
		case 2:
			hitsound = BASS::stream_sounds.ut_hitsound;
			break;
		case 3:
			hitsound = BASS::stream_sounds.q3_hitsound;
			break;
		case 4:
			hitsound = BASS::stream_sounds.roblox_hitsound;
			break;
		case 5:
			hitsound = BASS::stream_sounds.uff_hitsound;
			break;
		case 6:
			hitsound = BASS::stream_sounds.laser;
			break;
		default:
			break;
		}

		if (hitsound)
		{
			BASS_SET_VOLUME(hitsound, config.esp.hitsound_volume / 100.f);
			BASS_ChannelPlay(hitsound, true);
		}
	}
}
