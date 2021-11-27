#include "c_events.h"
#include "../c_rifk.h"
#include "../hacks/c_hitmarker.h"
#include "../hacks/c_resolver.h"
#include "../BASS/API.h"
#include "../sdk/c_debug_overlay.h"
#include "../sdk/c_view_beams.h"
#include "../hacks/c_esp.h"
#include "../sdk/c_weapon_system.h"

void c_events::hook()
{
	static c_events events {};
	game_event_manager()->add_listener_global(&events, false);
}

void c_events::fire_game_event(c_game_event* event)
{
	static auto delay = 0;

	if (delay > 0)
		++delay;

	// FIXME: refactor this into something nice.
	const auto buy_bot = []() -> void
	{
		if (config.rage.enabled && config.misc.buy_bot > 0)
		{
			if (config.misc.buy_bot == 1)
			{
				const auto local = c_cs_player::get_local_player();

				if (local)
				{
					bool buy_auto = true;
					auto weaponlist = local->get_current_weapons();

					if (weaponlist)
					{
						for (int i = 0; weaponlist[i]; i++)
						{
							const auto weapon = reinterpret_cast<c_base_combat_weapon*>(
								client_entity_list()->get_client_entity_from_handle(weaponlist[i]));

							if (!weapon)
								continue;

							const auto wpn_info = weapon_system->get_weapon_data(weapon->get_item_definition());
							if (!wpn_info)
								continue;

							if (wpn_info->get_weapon_id() == weapon_g3sg1 || wpn_info->get_weapon_id() == weapon_scar20)
							{
								buy_auto = false;
								break;
							}
						}
					}

					buy_auto ? engine_client()->clientcmd_unrestricted(
						_("buy g3sg1; buy scar20; buy deagle; buy vest; buy vesthelm; buy defuser; buy molotov; buy incgrenade; buy hegrenade; buy smokegrenade; buy taser; buy primammo; buy secammo;"), 0) :
						engine_client()->clientcmd_unrestricted(
							_("buy deagle; buy vest; buy vesthelm; buy defuser; buy molotov; buy incgrenade; buy hegrenade; buy smokegrenade; buy taser; buy primammo; buy secammo;"), 0);
				}
			}
			else if (config.misc.buy_bot == 2)
				engine_client()->clientcmd_unrestricted(
					_("buy ssg08; buy deagle; buy vest; buy vesthelm; buy defuser; buy molotov; buy incgrenade; buy hegrenade; buy smokegrenade; buy taser;"), 0);
			else
				engine_client()->clientcmd_unrestricted(
					_("buy awp; buy deagle; buy vest; buy vesthelm; buy defuser; buy molotov; buy incgrenade; buy hegrenade; buy smokegrenade; buy taser;"), 0);
		}
	};

	auto run_buy_bot = false;

	if (delay >= 50)
	{
		delay = 0;
		buy_bot();
		run_buy_bot = true;
	}

	switch (fnv1a_rt(event->get_name()))
	{
	case fnv1a("player_hurt"):
		c_hitmarker::on_player_hurt(event);
		c_resolver::on_player_hurt(event);
		break;
	case fnv1a("weapon_fire"):
		c_resolver::on_weapon_fire(event);
		break;
	case fnv1a("bullet_impact"):
		c_esp::draw_enemy_impact(event);
		c_resolver::on_bullet_impact(event);
		break;
	case fnv1a("round_end"):
		is_active_round = false;
		break;
	case fnv1a("round_start"):
		is_active_round = true;
		delay = 1;
		logging->info(_("-----------------------------------ROUND START--------------------------------------"));
		
		if (!run_buy_bot)
			buy_bot();
		break;
	default:
		break;
	}
}

int c_events::get_event_debug_id()
{
	return 42;
}
