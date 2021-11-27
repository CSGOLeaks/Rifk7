#pragma once

#include "../sdk/c_game_event_manager.h"

class c_events : public c_game_event_listener
{
public:
	static void hook();

	void fire_game_event(c_game_event* event) override;
	int get_event_debug_id() override;

	inline static bool is_active_round = false;
};
