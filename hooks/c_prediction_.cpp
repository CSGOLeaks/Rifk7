#include "c_prediction_.h"
#include "../utils/c_hook.h"
#include "c_client.h"
#include "../hacks/c_prediction_system.h"
#include "../hacks/c_antiaim.h"
#include "../sdk/c_input.h"
#include "../sdk/c_base_view_model.h"
#include "../utils/math.h"
#include <intrin.h>

void c_prediction_::hook()
{
	static c_hook<c_prediction> hook(prediction());
	_in_prediction = hook.apply<in_prediction_t>(14, in_prediction);
	_run_command = hook.apply<run_command_t>(19, run_command);
}

bool __fastcall c_prediction_::in_prediction(c_prediction* prediction, uint32_t)
{
	static const auto return_to_maintain_sequence_transitions = sig("client_panorama.dll", "84 C0 74 17 8B 87");

	if (_ReturnAddress() == return_to_maintain_sequence_transitions)
		return false;

	return _in_prediction(prediction);
}

void __fastcall c_prediction_::run_command(c_prediction* prediction, uint32_t, c_cs_player* player, c_user_cmd* cmd, c_move_helper* helper)
{
	static const std::map<int, int> pullout_sequence
	{
		{ 61, 8 },
		{ 38, 3 },
		{ 2, 7 },
		{ 36, 6 },
		{ 3, 3 },
		{ 64, 4 },
		{ 35, 5 },
		{ 25, 5 },
		{ 27, 2 },
		{ 28, 4 },
		{ 14, 4 },
		{ 34, 2 },
		{ 23, 3 },
		{ 24, 2 },
		{ 19, 2 },
		{ 26, 2 },
		{ 16, 5 },
		{ 40, 3 },
		{ 8, 2 },
		{ 9, 2 },
		{ 10, 2 },
		{ 31, 2 },
		{ 7, 4 },
		{ 4, 3 },
		{ 13, 2 },
		{ 11, 3 },
		{ 17, 5 },
		{ 29, 5 },
		{ 30, 5 },
		{ 49, 1 },
		{ 1, 6 },
		{ 32, 6 },
		{ 63, 4 },
		{ 33, 2 },
		{ 60, 8 },
		{ 43, 3 },
		{ 44, 3 },
		{ 45, 3 },
		{ 46, 3 },
		{ 47, 3 },
		{ 48, 3 },
		{ 42, 0 },
		{ 59, 0 },
		{ 500, 0 },
		{ 505, 0 },
		{ 506, 0 },
		{ 507, 0 },
		{ 508, 0 },
		{ 509, 0 },
		{ 512, 0 },
		{ 514, 0 },
		{ 515, 0 },
		{ 516, 0 },
		{ 519, 0 },
		{ 520, 0 },
		{ 522, 0 },
		{ 523, 0 }
	};

	if (!player->is_local_player() || !engine_client()->is_ingame())
	{
		_run_command(prediction, player, cmd, helper);
		return;
	}

	const auto prev_weapon = player->get_current_weapon_handle();

	const auto weapon = reinterpret_cast<c_base_combat_weapon*>(client_entity_list()->get_client_entity_from_handle(prev_weapon));

	if (weapon && weapon->get_item_definition() == weapon_revolver &&
		(antiaim->shot_cmd == cmd->command_number - 1 || antiaim->shot_cmd == cmd->command_number))
		weapon->get_postpone_fire_ready_time() = global_vars_base->curtime - ticks_to_time(2);

	_run_command(prediction, player, cmd, helper);
	
	if (prev_weapon != player->get_current_weapon_handle() && !input->camera_in_third_person && false)
	{
		const auto view_model = reinterpret_cast<c_base_view_model*>(
			client_entity_list()->get_client_entity_from_handle(player->get_view_model()));

		if (view_model)
		{
			auto weapon = reinterpret_cast<c_base_combat_weapon*>(
				client_entity_list()->get_client_entity_from_handle(view_model->get_weapon()));
			if (weapon && pullout_sequence.find(weapon->get_item_definition()) != pullout_sequence.end())
				view_model->send_matching_seq(pullout_sequence.at(weapon->get_item_definition()));
		}
	}

	c_client::recoil_info[player->get_tick_base() & 63] = {
		player->get_punch_angle(),
		player->get_punch_angle_vel(),
		player->get_view_offset()
	};

	antiaim->predict(player, cmd);
}
