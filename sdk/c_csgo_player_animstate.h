#pragma once

#include "c_base_entity.h"
#include "c_base_combat_weapon.h"

struct c_csgo_player_anim_state
{
	void reset()
	{
		static auto reset_anim_state = reinterpret_cast<void(__thiscall*)(c_csgo_player_anim_state*)>(
			sig("client_panorama.dll", "56 6A 01 68 ? ? ? ? 8B F1"));
		reset_anim_state(this);
	}

	char pad[3];
	char unknown;
	char pad2[91];
	c_base_entity* base_entity;
	c_base_combat_weapon* active_weapon;
	c_base_combat_weapon* last_active_weapon;
	float last_client_side_animation_update_time;
	int last_client_side_animation_update_framecount;
	float delta_time;
	float eye_yaw;
	float eye_pitch;
	float goal_feet_yaw;
	float current_feet_yaw;
	float current_torso_yaw;
	float lean_velocity;
	float lean_amount;
	char pad4[4];
	float feet_cycle;
	float feet_yaw_rate;
	float unknown2;
	float duck_amount;
	float landing_duck_additive_something;
	float unknown3;
	c_vector3d origin;
	c_vector3d last_origin;
	c_vector3d velocity;
	c_vector3d move_speed;
	c_vector3d move;
	float speed;
	float speed_up;
	float speed_normalized;
	float feet_speed_forwards_or_side_ways;
	float feet_speed_forwards_or_side_ways_fraction;
	float time_since_started_moving;
	float time_since_stopped_moving;
	unsigned char on_ground;
	unsigned char in_hit_ground_animation;
	char pad7[10];
	float last_origin_z;
	float head_height_or_offset_from_hitting_ground_animation;
	float stop_to_full_running_fraction;
	char pad8[4];
	float unknown_fraction;
	char pad9[4];
	float unknown5;
	char pad10[528];
};
