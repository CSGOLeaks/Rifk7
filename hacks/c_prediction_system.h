#pragma once
#include "../utils/c_singleton.h"
#include "../sdk/c_cs_player.h"
#include <cstdint>
#include "../sdk/c_game_movement.h"

class c_prediction_system : public c_singleton<c_prediction_system>
{
	using md5_pseudo_random = uint32_t(__thiscall*)(uint32_t);

	struct prediction_info
	{
		c_user_cmd* cmd{};
		float curtime{};
		c_move_data move_data{};
		int32_t flags{};
		c_base_handle ground_entity{};
	};

public:
	c_prediction_system();

	void initial(c_cs_player* local, c_user_cmd* cmd, int32_t seq);
	void repredict(c_cs_player* local, c_user_cmd* cmd);
	void restore() const;

	prediction_info animation_info[150]{};
	c_user_cmd original_cmd{};
	int32_t unpredicted_flags{};
	c_vector3d unpredicted_velocity{};

	c_move_data move_data{}, unpredicted_move_data{};

private:
	uint32_t* prediction_random_seed;
	c_cs_player** prediction_player;
	md5_pseudo_random _md5_pseudo_random;

	c_user_cmd* last_cmd{};
	int32_t tick_base{}, seq_diff{};

	float backup_curtime{}, backup_frametime{};
	int backup_tickbase{};
	c_vector3d backup_origin{};
	c_qangle backup_aim_punch{}, backup_aim_punch_velocity{}, backup_view_offset{};
	float backup_accuracy_penalty{}, backup_recoil_index{}, backup_duck_amount{};
};

#define prediction_system c_prediction_system::instance()
