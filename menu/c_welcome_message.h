#pragma once

#include "framework/c_text.h"

class c_welcome_message : public c_text
{
public:
	explicit c_welcome_message();

	void draw(c_vector2d position) override;

	void animate() override;

	c_vector2d get_size() override;

private:
	static auto constexpr fade_frequency = 255 / 1.5f;

	bool fade_direction;
	int fade;
};
