#include "c_client.h"
#include "../utils/c_hook.h"
#include "../hacks/c_animation_system.h"
#include "../sdk/c_client_entity_list.h"
#include "../sdk/c_game_rules.h"
#include "../sdk/c_weapon_system.h"
#include "../hacks/c_esp.h"
#include "c_net_channel_.h"
#include "../hacks/c_movement.h"
#include "../hacks/c_resolver.h"
#include "../hacks/debug.h"
#include "../hacks/c_miscellaneous.h"
#include "../hacks/c_ragebot.h"
#include "c_client.h"
#include "../hacks/c_prediction_system.h"
#include "../sdk/c_input.h"
#include "../hacks/c_antiaim.h"
#include "../sdk/c_client_state.h"
#include "../sdk/c_prediction.h"
#include "../sdk/c_base_view_model.h"
#include "../hacks/c_legitbot.h"
#include "c_client_state_.h"
#include "c_events.h"
#include "c_cl_camera_height_restriction_debug.h"

void c_client::hook()
{
	static c_hook<c_base_client> hook(base_client());
	_shutdown = hook.apply<shutdown_t>(4, shutdown);
	_level_init_pre_entity = hook.apply<level_init_pre_entity_t>(5, level_init_pre_entity);
	_create_move = hook.apply<create_move_t>(22, ::create_move);
	_frame_stage_notify = hook.apply<frame_stage_notify_t>(37, frame_stage_notify);
}

void __fastcall c_client::shutdown(c_base_client* client, uint32_t)
{
	/*
#ifdef RELEASE
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	const auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	std::pair<uint32_t, uint32_t> result;

	if (Process32First(snapshot, &entry))
		while (Process32Next(snapshot, &entry))
		{
			std::string name = entry.szExeFile;
			std::transform(name.begin(), name.end(), name.begin(), ::tolower);

			if (fnv1a_rt(name.c_str()) == fnv1a("csgo.exe"))
			{
				if (entry.cntThreads <= 1)
				{
					const auto process = OpenProcess(PROCESS_ALL_ACCESS, false, entry.th32ProcessID);
					if (process)
					{
						TerminateProcess(process, 0);
						CloseHandle(process);
					}
				}
			}
		}

	CloseHandle(snapshot);
#endif

	return _shutdown(client);
	*/
}

void c_client::level_init_pre_entity(c_base_client* client, uint32_t, const char* map_name)
{
	/*
	static const auto welcome_message = __("Now playing on: %s - Ping: %d ms");

	if (map_name)
	{
		char welcome[80];
		_rt(welcome_msg, welcome_message);
		sprintf_s(welcome, welcome_msg, map_name, static_cast<int>(net_channel->get_latency(flow_incoming) * 1000.f));
		logging->info(welcome);
	}

	c_cs_player::set_local_player(nullptr);
	c_net_channel_::apply_to_net_chan(client_state->net_channel);
	c_client_state_::cmds.clear();
	_level_init_pre_entity(client, map_name);
	*/
	c_net_channel_::apply_to_net_chan(client_state->net_channel);

	_level_init_pre_entity(client, map_name);

}


template<typename FuncType>
__forceinline static FuncType CallVFuckingFunc(void* ppClass, int index)
{
	int* pVTable = *(int**)ppClass;
	int dwAddress = pVTable[index];
	return (FuncType)(dwAddress);
}


c_user_cmd *c_input::GetUserCmd(int nSlot, int sequence_number)
{
	typedef c_user_cmd*(__thiscall *GetUserCmd_t)(void*, int, int);
	return CallVFuckingFunc<GetUserCmd_t>(this, 8)(this, nSlot, sequence_number);
}

void __stdcall c_client::create_move(int sequence_number, float input_sample_frametime, bool active, bool& sendpacket)
{
	static const auto get_check_sum = reinterpret_cast<uint32_t(__thiscall*)(c_user_cmd*)>(sig("client_panorama.dll", "53 8B D9 83 C8"));

	if (!get_check_sum) {
		cvar()->console_color_printf(false, c_color(255, 100, 100), "FAILED TO FIND `get_check_sum`\n");
	}
	_create_move(base_client(), sequence_number, input_sample_frametime, active);

	auto cmd = input->GetUserCmd(0, sequence_number);
	auto verified_cmd = &input->verified_commands[sequence_number % 150];
	if (!cmd || cmd == 0) return _create_move(base_client(), sequence_number, input_sample_frametime, active);

	const auto local = reinterpret_cast<c_cs_player*>(client_entity_list()->get_client_entity(engine_client()->get_local_player()));

	c_cl_camera_height_restriction_debug::in_cm = true;

	if (!cmd->command_number || !local || !local->is_alive())
	{
		c_cl_camera_height_restriction_debug::in_cm = false;
		return;
	}

	const auto weapon = reinterpret_cast<c_base_combat_weapon*>(
		client_entity_list()->get_client_entity_from_handle(local->get_current_weapon_handle()));

	c_base_combat_weapon::weapon_data* wpn_info = nullptr;

	if (!weapon || !(wpn_info = weapon_system->get_weapon_data(weapon->get_item_definition())))
	{
		c_cl_camera_height_restriction_debug::in_cm = false;
		return;
	}

	animation_system->in_jump = cmd->buttons & c_user_cmd::jump;

	if (!config.rage.enabled)
	{
		prediction_system->initial(local, cmd, sequence_number);
		c_legitbot::backtrack(local, cmd);

		if (local->is_shooting(cmd, global_vars_base->curtime))
		{
			// store shot info.
			resolver::shot shot{};
			shot.start = local->get_shoot_position();
			shot.time = global_vars_base->curtime;
			shot.manual = true;
			c_resolver::register_shot(std::move(shot));
		}

		animation_system->update_simple_local_player(local, cmd);
		prediction_system->restore();

		// verify new command.
		math::ensure_bounds(cmd->viewangles, *reinterpret_cast<c_vector3d*>(&cmd->forwardmove));
		verified_cmd->cmd = *cmd;
		verified_cmd->crc = get_check_sum(&verified_cmd->cmd);
		c_cl_camera_height_restriction_debug::in_cm = false;
		return;
	}

	cmd->buttons &= ~c_user_cmd::speed;

	c_movement::run(local, cmd);
	prediction_system->initial(local, cmd, sequence_number);
	auto target_angle = cmd->viewangles;

	// initialize revolver.
	local->can_shoot(cmd, global_vars_base->curtime);
	//antiaim->fakelag(local, cmd, sendpacket);
	const auto stopped_last_interval = c_ragebot::get_autostop_info().did_stop;
	c_ragebot::autostop(local, cmd);

	/*if (!client_state->choked_commands
		&& local->is_shooting(cmd, global_vars_base->curtime))
		cmd->buttons &= ~c_user_cmd::attack;
	else */if (local->is_shooting(cmd, global_vars_base->curtime))
	{
		// store shot info.
		resolver::shot shot{};
		shot.start = local->get_shoot_position();
		shot.time = global_vars_base->curtime;
		shot.manual = true;
		c_resolver::register_shot(std::move(shot));
	}
	//else if (client_state->choked_commands && !local->is_shooting(cmd, global_vars_base->curtime))
	c_ragebot::aim(local, cmd, sendpacket);

	if (local->is_shooting(cmd, global_vars_base->curtime))
	{
		c_aimhelper::fix_movement(cmd, target_angle);
		antiaim->shot_cmd = cmd->command_number;
		sendpacket = true;
	}

	if ((!client_state->choked_commands && stopped_last_interval)
		|| (stopped_last_interval && c_cs_player::can_not_shoot_due_to_cock && weapon->get_item_definition() == weapon_revolver))
		c_ragebot::autostop(local, cmd);

	if (!c_ragebot::get_autostop_info().did_stop)
	{
		cmd->forwardmove = prediction_system->original_cmd.forwardmove;
		cmd->sidemove = prediction_system->original_cmd.sidemove;
		prediction_system->repredict(local, cmd);
	}

	static const auto recoil_scale = cvar()->find_var(_("weapon_recoil_scale"));
	cmd->viewangles -= local->get_punch_angle() * recoil_scale->get_float();

	/*
	if (sendpacket)
	{
		antiaim->run(local, cmd);
		c_client_state_::cmds.push_back(cmd->command_number);
	}
	else // keep up communication even when fakelagging.
	{
		const auto current_choke = net_channel->choked_packets;
		net_channel->choked_packets = 0;
		net_channel->send_datagram();
		--net_channel->out_sequence_nr;
		net_channel->choked_packets = current_choke;
	}
	*/

	antiaim->run(local, cmd);
	c_client_state_::cmds.push_back(cmd->command_number);
	prediction_system->repredict(local, cmd);
	prediction_system->restore();

	// remove duck stamina.
	if (config.rage.infinite_duck)
		c_miscellaneous::remove_duck_stamina(cmd);

	// verify new command.
	math::ensure_bounds(cmd->viewangles, *reinterpret_cast<c_vector3d*>(&cmd->forwardmove));
	c_miscellaneous::set_buttons_for_direction(cmd);
	verified_cmd->cmd = *cmd;
	verified_cmd->crc = get_check_sum(&verified_cmd->cmd);
	c_cl_camera_height_restriction_debug::in_cm = false;
}

/*


	

	

	if (!client_state->choked_commands
		&& local->is_shooting(cmd, global_vars_base->curtime))
		cmd->buttons &= ~c_user_cmd::attack;
	else if (local->is_shooting(cmd, global_vars_base->curtime))
	{
		// store shot info.
		resolver::shot shot{};
		shot.start = local->get_shoot_position();
		shot.time = global_vars_base->curtime;
		shot.manual = true;
		c_resolver::register_shot(std::move(shot));
	}
	else if (client_state->choked_commands && !local->is_shooting(cmd, global_vars_base->curtime))
		c_ragebot::aim(local, cmd, sendpacket);

	if (local->is_shooting(cmd, global_vars_base->curtime))
	{
		c_aimhelper::fix_movement(cmd, target_angle);
		antiaim->shot_cmd = cmd->command_number;
		sendpacket = true;
	}

	if ((!client_state->choked_commands && stopped_last_interval)
		|| (stopped_last_interval && c_cs_player::can_not_shoot_due_to_cock && weapon->get_item_definition() == weapon_revolver))
		c_ragebot::autostop(local, cmd);

	if (!c_ragebot::get_autostop_info().did_stop)
	{
		cmd->forwardmove = prediction_system->original_cmd.forwardmove;
		cmd->sidemove = prediction_system->original_cmd.sidemove;
		prediction_system->repredict(local, cmd);
	}

	static const auto recoil_scale = cvar()->find_var(_("weapon_recoil_scale"));
	cmd->viewangles -= local->get_punch_angle() * recoil_scale->get_float();

	if (sendpacket)
	{
		antiaim->run(local, cmd);
		c_client_state_::cmds.push_back(cmd->command_number);
	}
	else // keep up communication even when fakelagging.
	{
		const auto current_choke = net_channel->choked_packets;
		net_channel->choked_packets = 0;
		net_channel->send_datagram();
		--net_channel->out_sequence_nr;
		net_channel->choked_packets = current_choke;
	}

	prediction_system->repredict(local, cmd);
	prediction_system->restore();

	// remove duck stamina.
	if (config.rage.infinite_duck)
		c_miscellaneous::remove_duck_stamina(cmd);

	// verify new command.
	math::ensure_bounds(cmd->viewangles, *reinterpret_cast<c_vector3d*>(&cmd->forwardmove));
	c_miscellaneous::set_buttons_for_direction(cmd);
	verified_cmd->cmd = *cmd;
	verified_cmd->crc = get_check_sum(&verified_cmd->cmd);
	c_cl_camera_height_restriction_debug::in_cm = false;

*/

void c_client::frame_stage_notify(c_base_client* client, uint32_t, const clientframestage stage)
{
	
	if (!engine_client()->is_ingame())
		return _frame_stage_notify(client, stage);

	const auto local = c_cs_player::get_local_player();

	static auto cycle = 0.f;
	static auto anim_time = 0.f;

	const auto view_model = local ? reinterpret_cast<c_base_view_model*>(
		client_entity_list()->get_client_entity_from_handle(local->get_view_model())) : nullptr;

	if (stage == frame_render_start)
	{
		c_resolver::on_render_start();

		if (local && local->is_alive())
			local->set_abs_angles(animation_system->local_animation.abs_ang);
	}

	_frame_stage_notify(client, stage);

	if (stage == frame_render_start)
	{
		antiaim->increment_visual_progress();
		c_esp::store_data();
		c_miscellaneous::set_viewmodel_parameters();
		c_miscellaneous::remove_flash();
		c_miscellaneous::remove_smoke();
		c_miscellaneous::engine_radar();
	}

	if (stage == frame_net_update_postdataupdate_start && local && local->is_alive())
	{
		// restore local player data.
		const auto& info = recoil_info[local->get_tick_base() & 63];

		if (fabsf(info.aim_punch.y - local->get_punch_angle().y) < 0.5f &&
			fabsf(info.aim_punch.x - local->get_punch_angle().x) < 0.5f &&
			fabsf(info.aim_punch.z - local->get_punch_angle().z) < 0.5f)
			local->get_punch_angle() = info.aim_punch;

		if (fabsf(info.aim_punch_vel.y - local->get_punch_angle_vel().y) < 0.5f &&
			fabsf(info.aim_punch_vel.x - local->get_punch_angle_vel().x) < 0.5f &&
			fabsf(info.aim_punch_vel.z - local->get_punch_angle_vel().z) < 0.5f)
			local->get_punch_angle_vel() = info.aim_punch_vel;

		if (fabsf(info.view_offset.y - local->get_view_offset().y) < 0.5f &&
			fabsf(info.view_offset.x - local->get_view_offset().x) < 0.5f &&
			fabsf(info.view_offset.z - local->get_view_offset().z) < 0.5f)
			local->get_view_offset() = info.view_offset;

		if (view_model && !input->camera_in_third_person)
		{
			view_model->get_anim_time() = anim_time;
			view_model->get_cycle() = cycle;
		}

		animation_system->server_layers = *local->get_animation_layers();
	}

	if (view_model)
	{
		cycle = view_model->get_cycle();
		anim_time = view_model->get_anim_time();
	}

	if (stage == frame_net_update_end)
		animation_system->post_player_update();
}

__declspec(naked) void create_move(int sequence_number, float input_sample_frametime, bool active)
{
	__asm
	{
		push ebx
		push esp
		push[esp + 0x14]
		push[esp + 0x14]
		push[esp + 0x14]
		call c_client::create_move
		pop ebx
		ret 0x0c
	}
}
