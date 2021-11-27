#pragma once

#include "../sdk/c_game_event_manager.h"

class c_game_event_manager_
{
	typedef bool(__thiscall* add_listener_t)(c_game_event_manager*, c_game_event_listener*, const char*, bool);

public:
    static void hook();

private:
	inline static add_listener_t _add_listener;

	static bool __stdcall add_listener(c_game_event_listener* listener, const char* name, bool serverside);
};
