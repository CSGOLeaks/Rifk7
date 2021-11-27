#pragma once

#include "../includes.h"

class c_antiaim : public c_singleton<c_antiaim>
{
public:
	void fakelag(c_cs_player* local, c_user_cmd* cmd, bool& send_packet);
	void run(c_cs_player* local, c_user_cmd* cmd);
	void prepare_animation(c_cs_player* local);
	void predict(c_cs_player* local, c_user_cmd* cmd);

	float get_visual_choke();
	void increment_visual_progress();

	float get_last_real();
	float get_last_fake();

	uint32_t shot_cmd{};
private:
	bool on_peek(c_cs_player* local, bool& target);
	float calculate_ideal_yaw(c_cs_player* local, bool estimate = false);

	float visual_choke = 0.f,
		last_real = 0.f, last_fake = 0.f,
		next_lby_update = 0.f, lby_update = 0.f,
		min_delta = 0.f, max_delta = 0.f,
		stop_to_full_running_fraction = 0.f,
		feet_speed_stand = 0.f, feet_speed_ducked = 0.f;
	bool is_standing = false;
	uint32_t estimated_choke = 0;
};

#define antiaim c_antiaim::instance()
