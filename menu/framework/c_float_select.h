#pragma once

#include "c_select.h"

class c_float_select : public c_select
{
public:
	c_float_select(std::pair<std::string, char> title, float& selected, float max = 100.f, float min = .0f, float step = .25f);

	void draw(c_vector2d position) override;
	void on_change() override;

private:
	int min, max, currently_selected;
	float& float_selected;
	float step;
};
