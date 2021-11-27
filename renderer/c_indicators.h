#pragma once
#include "../sdk/c_vector2d.h"

class c_color;

class c_indicators
{
public:
	static void draw();

private:
	static void draw_radio_info(c_vector2d& position);
	static void draw_ping_acceptance(c_vector2d& position);
	static void draw_choke(c_vector2d& position);

	static void draw_antiaim();

	static void draw_progressbar(c_vector2d& position, const char* name, c_color color1, c_color color2, float progress);
	static void draw_arrow(float angle, c_color color);
};
