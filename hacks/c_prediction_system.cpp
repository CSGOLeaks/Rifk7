#include "c_prediction_system.h"
#include "../utils/c_memory.h"
#include "../utils/math.h"
#include "../sdk/c_global_vars_base.h"
#include "../sdk/c_client_entity_list.h"
#include "../sdk/c_move_helper.h"
#include "../sdk/c_prediction.h"
#include "../sdk/c_cvar.h"
#include "../sdk/c_client_state.h"
#include "../sdk/c_input.h"
#include "c_animation_system.h"

c_prediction_system::c_prediction_system()
{
	prediction_random_seed = *reinterpret_cast<uint32_t**>(reinterpret_cast<uint32_t>(
		sig("client_panorama.dll", "A3 ? ? ? ? 66 0F 6E 86")) + 1);
	prediction_player = *reinterpret_cast<c_cs_player***>(reinterpret_cast<uint32_t>(
		sig("client_panorama.dll", "89 35 ? ? ? ? F3 0F 10 48")) + 2);
	_md5_pseudo_random = reinterpret_cast<md5_pseudo_random>(sig("client_panorama.dll", "55 8B EC 83 E4 F8 83 EC 70 6A"));
}

void c_prediction_system::initial(c_cs_player* local, c_user_cmd* cmd, int32_t seq)
{
	const auto weapon = reinterpret_cast<c_base_combat_weapon*>(
		client_entity_list()->get_client_entity_from_handle(local->get_current_weapon_handle()));

	if (!weapon)
		return;

	// backup globals.
	backup_curtime = global_vars_base->curtime;
	backup_frametime = global_vars_base->frametime;

	// correct tickbase.
	if (!last_cmd || last_cmd->predicted)
		seq_diff = seq - local->get_tick_base();

	tick_base = max(local->get_tick_base(), seq - seq_diff);
	last_cmd = cmd;

	// setup globals.
	global_vars_base->curtime = ticks_to_time(tick_base);
	global_vars_base->frametime = local->get_flags() & c_base_player::frozen ? 0 : global_vars_base->interval_per_tick;

	// backup player data.
	backup_tickbase = local->get_tick_base();
	backup_origin = local->get_origin();
	backup_aim_punch = local->get_punch_angle();
	backup_aim_punch_velocity = local->get_punch_angle_vel();
	backup_view_offset = local->get_view_offset();
	backup_accuracy_penalty = weapon->get_accuracy_penalty();
	backup_recoil_index = weapon->get_recoil_index();
	backup_duck_amount = local->get_duck_amount();

	// backup flags, velocity and user cmd.
	original_cmd = *cmd;
	unpredicted_flags = local->get_flags();
	unpredicted_velocity = local->get_velocity();

	// simulate local player.
	local->run_pre_think();
	local->run_think();

	// setup movement data.
	memset(&move_data, 0, sizeof(c_move_data));
	prediction()->setup_move(local, cmd, move_helper, &move_data);
	unpredicted_move_data = move_data;

	// store animation move data.
	animation_info[cmd->command_number % 150] = { cmd, global_vars_base->curtime, unpredicted_move_data,
		local->get_flags(), local->get_ground_entity() };

	// setup prediction parameters.
	*prediction_player = local;

	// predict the local player.
	repredict(local, cmd);	
}

void c_prediction_system::repredict(c_cs_player* local, c_user_cmd* cmd)
{
	static const auto sv_footsteps = cvar()->find_var(_("sv_footsteps"));

	const auto weapon = reinterpret_cast<c_base_combat_weapon*>(
		client_entity_list()->get_client_entity_from_handle(local->get_current_weapon_handle()));

	if (!weapon)
		return;

	// backup footsteps.
	const auto backup_footsteps = sv_footsteps->get_int();
	sv_footsteps->flags &= ~(1 << 14); // FCVAR_CHEAT
	sv_footsteps->flags &= ~(1 << 8); // FCVAR_NOTIFY
	sv_footsteps->set_value(0);

	// select correct data.
	move_data = animation_info[cmd->command_number % 150].move_data;
	global_vars_base->curtime = animation_info[cmd->command_number % 150].curtime;

	// setup prediction parameters.
	*prediction_random_seed = _md5_pseudo_random(cmd->command_number) & 0x7FFFFFFF;

	// set player data.	
	local->get_punch_angle() = backup_aim_punch;
	local->get_punch_angle_vel() = backup_aim_punch_velocity;
	local->get_view_offset() = backup_view_offset;
	weapon->get_accuracy_penalty() = backup_accuracy_penalty;
	weapon->get_recoil_index() = backup_recoil_index;
	local->get_duck_amount() = backup_duck_amount;

	// start of prediction.
	move_helper->set_host(local);
	game_movement()->start_track_prediction_errors(local);

	// correct move data.
	move_data.forward_move = cmd->forwardmove;
	move_data.side_move = cmd->sidemove;
	move_data.up_move = cmd->upmove;
	move_data.buttons = cmd->buttons;
	move_data.view_angles = cmd->viewangles;
	move_data.angles = cmd->viewangles;
	move_data.impulse_command = cmd->impulse;

	// restore player state.
	prediction()->finish_move(local, animation_info[cmd->command_number % 150].cmd, &move_data);
	local->get_flags() = animation_info[cmd->command_number % 150].flags;
	local->get_ground_entity() = animation_info[cmd->command_number % 150].ground_entity;

	// run movement.
	game_movement()->process_movement(local, &move_data);

	// finish movement.
	prediction()->finish_move(local, cmd, &move_data);

	// correct player data.
	local->get_tick_base() = backup_tickbase;
	local->get_velocity() = (local->get_origin() - backup_origin) * (1.f / global_vars_base->interval_per_tick);

	// end of prediction.
	move_helper->unknown_func();
	game_movement()->finish_track_prediction_errors(local);
	move_helper->set_host(nullptr);

	// update weapon accuracy.
	weapon->update_accuracy();

	// restore footsteps.
	sv_footsteps->set_value(backup_footsteps);
}

void c_prediction_system::restore() const
{
	// restore globals.
	*prediction_random_seed = -1;
	*prediction_player = nullptr;
	global_vars_base->curtime = backup_curtime;
	global_vars_base->frametime = backup_frametime;
}
