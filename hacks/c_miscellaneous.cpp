#include "c_miscellaneous.h"
#include "../utils/c_memory.h"
#include "../sdk/c_material_system.h"
#include <cstdint>
#include "../sdk/c_engine_client.h"
#include "../utils/c_config.h"
#include "../utils/c_hotkeys.h"
#include "../sdk/c_cs_player.h"
#include "../sdk/c_input.h"
#include "../sdk/c_cvar.h"
#include "../menu/c_menu.h"

void c_miscellaneous::disable_post_processing()
{
	if (!engine_client()->is_ingame() || !engine_client()->is_connected())
		return;

	static auto& disable_post_processing_override = **reinterpret_cast<bool**>(reinterpret_cast<uint32_t>(
		sig("client_panorama.dll", "83 EC 4C 80 3D") + 5));

	static auto blur_overlay = find_mat("dev/scope_bluroverlay");
	static auto lens_dirt = find_mat("models/weapons/shared/scope/scope_lens_dirt");

	disable_post_processing_override = true;
	blur_overlay->set_material_var_flag(material_var_no_draw, true);
	lens_dirt->set_material_var_flag(material_var_no_draw, true);
}

void c_miscellaneous::unlock_client_command_buffer()
{
	static const auto new_commands = reinterpret_cast<uint8_t*>(reinterpret_cast<uint32_t>(
		sig("engine.dll", "55 8B EC A1 ? ? ? ? 81 EC ? ? ? ? B9 ? ? ? ? 53 8B 98")) + 0xbd);

	if ((config.rage.enabled && *new_commands == 24)
		|| (!config.rage.enabled && *new_commands == 15))
		return;

	DWORD old;
	PVOID address = new_commands;
	ULONG size = sizeof(uintptr_t);
	syscall(NtProtectVirtualMemory)(current_process, &address, &size, PAGE_READWRITE, &old);
	address = new_commands;
	size = sizeof(uintptr_t);
	*new_commands = config.rage.enabled ? 24 : 15;
	syscall(NtProtectVirtualMemory)(current_process, &address, &size, old, &old);
}

void c_miscellaneous::remove_visual_recoil(c_view_setup& view)
{
	const auto local = c_cs_player::get_local_player();

	if (!local || !local->is_alive() || input->camera_in_third_person || !config.rage.enabled)
		return;

	view.angles -= local->get_punch_angle() * .9f + local->get_view_punch_angle();
}

void c_miscellaneous::change_fov(c_view_setup& view)
{
	const auto local = c_cs_player::get_local_player();

	if (!local || !local->is_alive())
		return;

	view.fov += config.misc.fov - 50.f;

	if (!input->camera_in_third_person)
		view.fov_viewmodel += config.misc.fov_view - 50.f;
}

void c_miscellaneous::set_viewmodel_parameters()
{
	static const auto cl_wpn_sway_interp = cvar()->find_var(_("cl_wpn_sway_interp"));
	static const auto cl_bob_lower_amt = cvar()->find_var(_("cl_bob_lower_amt"));

	if (config.rage.enabled)
	{
		cl_wpn_sway_interp->value = 0.f;
		cl_bob_lower_amt->value = 0.f;
	}
	else
	{
		cl_wpn_sway_interp->value = 5.f;
		cl_bob_lower_amt->value = 1.6f;
	}
}

void c_miscellaneous::set_camera_to_thirdperson()
{
	static auto in_thirdperson = false;
	static auto require_reset = false;

	static const auto register_callback = []() -> bool {
		hotkeys->register_callback([](uint32_t code) {
			if (code != 0 && code == config.misc.thirdperson_switch && !menu->is_open())
				in_thirdperson = !in_thirdperson;
		});
		return true;
	} ();

	static const auto cam_idealdist = cvar()->find_var(_("cam_idealdist"));
	static const auto cam_collision = cvar()->find_var(_("cam_collision"));

	const auto local = c_cs_player::get_local_player();

	if (!engine_client()->is_ingame() || !local)
		return;

	static auto percent = 0.f;
	if (local->is_alive() && in_thirdperson)
	{
		input->camera_in_third_person = true;

		percent = std::clamp(percent + global_vars_base->frametime * 8.f, 0.f, 1.f);

		cam_idealdist->set_value((config.misc.thirdperson_dist + 20.f) * percent);
		cam_collision->set_value(1);
	}
	else if (input->camera_in_third_person)
	{
		percent = in_thirdperson ? 1.f : 0.f;
		input->camera_in_third_person = false;
	}

	if (local->is_alive() || !config.rage.enabled)
	{
		require_reset = false;
		return;
	}

	if (require_reset)
		local->get_observer_mode() = 5;

	if (local->get_observer_mode() == 4)
		require_reset = true;
}

void c_miscellaneous::remove_smoke()
{
	static const auto smoke_count = *reinterpret_cast<uint32_t**>(uint32_t(sig("client_panorama.dll", "A3 ? ? ? ? 57 8B CB")) + 1);

	if (!config.misc.no_smoke)
		return;

	*smoke_count = 0;
}

void c_miscellaneous::remove_flash()
{
	if (!config.misc.no_flash)
		return;

	const auto local = c_cs_player::get_local_player();

	if (!local || !local->is_alive())
		return;

	local->get_flash_alpha() = 0.f;
}

void c_miscellaneous::set_buttons_for_direction(c_user_cmd * cmd)
{
	cmd->buttons &= ~c_user_cmd::forward;
	cmd->buttons &= ~c_user_cmd::back;
	if (cmd->forwardmove > 0.f)
		cmd->buttons |= c_user_cmd::forward;
	else if (cmd->forwardmove < 0.f)
		cmd->buttons |= c_user_cmd::back;

	cmd->buttons &= ~c_user_cmd::move_right;
	cmd->buttons &= ~c_user_cmd::move_left;
	if (cmd->sidemove > 0.f)
		cmd->buttons |= c_user_cmd::move_right;
	else if (cmd->sidemove < 0.f)
		cmd->buttons |= c_user_cmd::move_left;
}

void c_miscellaneous::remove_duck_stamina(c_user_cmd * cmd)
{
	cmd->buttons |= c_user_cmd::bull_rush;
}

void c_miscellaneous::engine_radar()
{
	if (!config.misc.engine_radar)
		return;

	client_entity_list()->for_each_player([](c_cs_player* player) -> void
	{
		if (!player->is_dormant() && player->is_alive())
			player->is_spotted() = true;
	});
}
