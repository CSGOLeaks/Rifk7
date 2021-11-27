#pragma once

#include "macros.h"

class c_cs_game_rules_proxy_proxy
{
public:
	static c_cs_game_rules_proxy_proxy* get()
	{
		static const auto proxy = *reinterpret_cast<c_cs_game_rules_proxy_proxy***>(
			sig("client_panorama.dll", "A1 ? ? ? ? 85 C0 0F 84 ? ? ? ? 80 B8 ? ? ? ? ? 0F 84 ? ? ? ? 0F 10 05") + 0x1);

		return *proxy;
	}

	offset(is_freeze_period(), bool, 0x20)
	offset(is_valve_ds(), bool, 0x75)
	offset(is_bomb_planed(), bool, 0x8D1)
	offset(is_bomb_dropped(), bool, 0x8D0)

	vfunc(28, should_collide(const int group, const int group_alt), bool(__thiscall*)(void*, int, int))(group, group_alt)
};

#define game_rules c_cs_game_rules_proxy_proxy::get()