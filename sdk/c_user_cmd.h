#pragma once

#include "c_vector3d.h"
#include <cstdint>

class c_user_cmd
{
public:
	enum flags
	{
		attack = 1 << 0,
		jump = 1 << 1,
		duck = 1 << 2,
		forward = 1 << 3,
		back = 1 << 4,
		use = 1 << 5,
		cancel = 1 << 6,
		left = 1 << 7,
		right = 1 << 8,
		move_left = 1 << 9,
		move_right = 1 << 10,
		attack2 = 1 << 11,
		run = 1 << 12,
		reload = 1 << 13,
		alt1 = 1 << 14,
		alt2 = 1 << 15,
		score = 1 << 16,
		speed = 1 << 17,
		walk = 1 << 18,
		zoom = 1 << 19,
		weapon1 = 1 << 20,
		weapon2 = 1 << 21,
		bull_rush = 1 << 22,
		grenade1 = 1 << 23,
		grenade2 = 1 << 24
	};

	virtual ~c_user_cmd() = default;

	int command_number;
    int tick_count;
    c_vector3d viewangles;
    c_vector3d aim_direction;
    float forwardmove;
	float sidemove;
	float upmove;
    int buttons;
    uint8_t impulse;
    int weapon_select;
    int weapon_select_subtype;
    int random_seed;
    short mousedx;
    short mousedy;
    bool predicted;
private:
	char pad[0x18];
};

class c_verified_user_cmd
{
public:
	c_user_cmd cmd;
	unsigned long crc;
};
