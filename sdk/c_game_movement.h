#pragma once

#include "c_cs_player.h"

class c_move_data {  // NOLINT(hicpp-member-init)
public:
	bool first_run_of_functions : 1;
	bool game_code_moved_player : 1;
	bool no_air_control : 1;

	unsigned long player_handle{};
	int impulse_command{};
	c_qangle view_angles;
	c_qangle abs_view_angles;
	int	buttons{};
	int	old_buttons{};
	float forward_move{};
	float side_move{};
	float up_move{};

	float max_speed{};
	float client_max_speed{};

	c_vector3d velocity;
	c_vector3d old_velocity;
	float somefloat{};
	c_qangle angles;
	c_qangle old_angles;

	float out_step_height{};
	c_vector3d out_wish_velocity;
	c_vector3d out_jump_velocity;

	c_vector3d constraint_center;
	float constraint_radius{};
	float constraint_width{};
	float constraint_speed_factor{};
	bool constraint_past_radius{};

	c_qangle abs_origin;
};

class c_game_movement {
public:
	virtual	~c_game_movement() = default;
	virtual void process_movement(c_cs_player* player, c_move_data* data) = 0;
	virtual void reset() = 0;
	virtual void start_track_prediction_errors(c_cs_player* player) = 0;
	virtual void finish_track_prediction_errors(c_cs_player* player) = 0;
};

interface_var(c_game_movement, game_movement, "client_panorama.dll", "GameMovement")
