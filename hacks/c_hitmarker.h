#pragma once

#include "../sdk/c_game_event_manager.h"

class c_hitmarker
{
public:
	static void draw();
	static void on_player_hurt(c_game_event* event);

private:
	static int hit_alpha;
};
