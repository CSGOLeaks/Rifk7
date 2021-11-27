#pragma once

#include <functional>
#include "../utils/c_config.h"
#include "../sdk/c_cs_player.h"

class c_chams : public c_singleton<c_chams>
{
public:
	c_chams();

	void latch_timer();

	static void draw_players();
	static bool get_backtrack_matrix(c_cs_player* player, matrix3x4* out);

	static void player_chams(std::function<void()> original, c_config::config_conf::chams_conf::chams& conf);
	static void viewmodel_chams(std::function<void()> original, c_config::config_conf::chams_conf::chams& conf);
	
	static void set_ignorez(bool enabled = true);

	c_cs_player* current_player;
	matrix3x4* current_matrix;
	bool second_pass;

private:
	static void modulate(c_color color, c_material* material = nullptr);
	static void modulate_exp(c_material* material, float alpha = 1.f, float width = 6.f);

	int alpha;
	bool direction;
};
