#pragma once

#include "../includes.h"

class c_legitbot
{
public:
	static void aim(float* x, float* y);
	static void backtrack(c_cs_player* local, c_user_cmd* cmd);

private:
	static void smooth(c_qangle& end_angles, const c_qangle& prev_angles);
	static c_vector2d angle_to_pixels(const c_qangle& angle);
	static c_qangle pixels_to_angle(const c_vector2d& pixel);
};
